//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>
#include <vfs.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <copyinout.h>
#include <limits.h>
#include <proc.h>
#include <addrspace.h>

int sys_execv(const_userptr_t progname, const_userptr_t args) {

    size_t fname_size;
    int result;
    char *k_progname;
    struct vnode *v;
    int arg_c;
    const_userptr_t *arg_ptrs;
    char **k_args;
    struct addrspace *as;
    vaddr_t entrypoint, stackptr;

    if (progname == NULL) {
        return ENOENT;
    }

    /*
     * OPEN THE VFS FILE WITH THE PROGRAM
     */
    k_progname = kmalloc(MAX_FILENAME_SIZE_T);
    if (k_progname == NULL) { return ENOMEM;}
    copyinstr(progname, k_progname, MAX_FILENAME_LEN, &fname_size);
    result = vfs_open(k_progname, O_RDONLY, 0, &v);
    if (result) {
        return result;
    }

    /**
     * GET THE ARGS
     */

    /*
     * GET NUMBER OF ARG POINTERS
     */
    arg_c = 0;
    const_userptr_t next_arg = kmalloc(sizeof(const_userptr_t));
    copyin(args, &next_arg, sizeof(const_userptr_t));
    while (next_arg != NULL) {
        arg_c++;
        copyin(args + sizeof(const_userptr_t)*arg_c, &next_arg, sizeof(const_userptr_t));
    }

    /*
     * ALLOCATE AN ARGS_PTR ARRAY OF CORRECT SIZE
     */
    arg_ptrs = kmalloc(sizeof(const_userptr_t) * arg_c);

    /*
     * COPY EACH ARG POINTER INTO THE ARRAY
     */
    for (int i=0; i<arg_c; i++) {
        copyin(args + sizeof(const_userptr_t)*i, &arg_ptrs[i], sizeof(const_userptr_t));
    }

    /*
     * ALLOCATE AND FILL CHAR** ARRAY of ARGS
     */
    k_args = kmalloc(sizeof(char *) * arg_c);
    for (int i=0; i<arg_c; i++) {
        k_args[i] = kmalloc(ARG_MAX_LEN);
        copyinstr(arg_ptrs[i], k_args[i], ARG_MAX_LEN, NULL);
        if (k_args[i][ARG_MAX_LEN-1] != '\000') {
            kfree(arg_ptrs);
            kfree(k_args);
            kfree(&next_arg);
            kfree(k_progname);
            return E2BIG;
        }
    }

    /* Create a new address space. */
    as = as_create();
    if (as == NULL) {
        vfs_close(v);
        return ENOMEM;
    }

    /* Switch to it and activate it. */
    proc_setas(as);
    as_activate();

    /* Load the executable. */
    result = load_elf(v, &entrypoint);
    if (result) {
        /* p_addrspace will go away when curproc is destroyed */
        vfs_close(v);
        return result;
    }
    as_complete_load(as);

    /* Done with the file now. */
    vfs_close(v);

    /* Define the user stack in the address space */
    result = as_define_stack(as, &stackptr);
    if (result) {
        /* p_addrspace will go away when curproc is destroyed */
        return result;
    }

    /*
     * PLACE ARGUMENTS ON STACK
     */
    for (int i=0; i<arg_c; i++) {
        copyoutstr(k_args[i], (userptr_t) (stackptr + ARG_MAX_LEN*i), ARG_MAX_LEN, NULL);
    }



    /* Warp to user mode. */
    enter_new_process(0 /*argc*/, NULL /*userspace addr of argv*/,
                      NULL /*userspace addr of environment*/,
                      stackptr, entrypoint);

    /* enter_new_process does not return. */
    panic("enter_new_process returned\n");
    return EINVAL;
}