//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>
#include <current.h> // For curproc
#include <proc.h>
#include <addrspace.h>
#include <kern/errno.h>
#include <synch.h>
#include <thread.h>
#include <mips/trapframe.h>
//#include "../arch/mips/thread/switchframe.h"

int sys_fork(struct trapframe *tf, pid_t *pid) {

    struct addrspace *new_addrspace;
    struct proc *new_proc;
//    struct trapframe *child_tf;
    int result;

    /*
     * ASSERTS AND PROLOGUE CHECKS
     */
    KASSERT(curproc != NULL);

    if (GLOBAL_PROC_COUNT >= MAX_PROCS) {
        return ENPROC;
    }

    if (curproc->p_num_childs >= MAX_CHILDS) {
        return EMPROC;
    }


    /*
     * CREATE A NEW PROCESS STRUCT
     */
    new_proc = kmalloc(sizeof(struct proc));
    if (new_proc == NULL) {
        return ENOMEM;
    }
    // Get the new PID
    new_proc->pid = GLOBAL_PROC_COUNT + 1;
    // Keep old proc's cwd
    new_proc->p_cwd = curproc->p_cwd;

    /*
     * COPY ADDRESS SPACE
     */
    new_addrspace = kmalloc(sizeof(struct addrspace));
    if (new_addrspace == NULL) {
        kfree(new_proc);
        return ENOMEM;
    }
    as_copy(curproc->p_addrspace, &new_addrspace);
    new_proc->p_addrspace = new_addrspace;


    /*
     * COPY PARENT'S TRAPFRAME
     */
//    child_tf = kmalloc(sizeof(struct trapframe));
//    if (child_tf == NULL) {
//        kfree(new_proc->p_addrspace);
//        kfree(new_proc);
//        return ENOMEM;
//    }
//    memcpy(child_tf, tf, sizeof(struct trapframe));

    // TODO (James): Copy local file table

    result = thread_fork("user_proc_thread", new_proc, enter_forked_process, tf, new_proc->pid);

    if (result) {

        return result;
    }

    *pid = curproc->pid;

    return 0;
}

/*
 * Implementation of a helper process for sys_fork()
 */
void enter_forked_process(void *tf, unsigned long pid) {

    struct trapframe *new_tf = kmalloc(sizeof(struct trapframe));
    memcpy(new_tf, tf, sizeof(struct trapframe));
    (void) pid;



    // Based on usage in the syscall method of syscall.c, these should work:
    // Register v0 in the trapframe is the syscall value OR the return value, in this case
    new_tf->tf_v0 = 0;

    // Apparently register a3 in the trapframe is used for error checking, so this indicates no error
    new_tf->tf_a3 = 0;

    // Increment the program counter so that we don't trap forever
    new_tf->tf_epc += 4;

    mips_usermode(new_tf);

    // This theoretically shouldn't be reachable
    KASSERT(1 == 2);
}