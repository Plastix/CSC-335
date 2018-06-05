#include <types.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <addrspace.h>
#include <kern/errno.h>
#include <synch.h>
#include <mips/trapframe.h>

int sys_fork(struct trapframe *tf, pid_t *pid) {
    struct proc *new_proc;
    struct trapframe *new_tf;
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
    new_proc = proc_create("user_proc");
    if (new_proc == NULL) {
        return ENOMEM;
    }

    lock_acquire(curproc->p_mutex);

    /*
     * KEEP PARENT CWD
     */
    new_proc->p_cwd = curproc->p_cwd;
    VOP_INCREF(new_proc->p_cwd);

    /*
     * COPY ADDRESS SPACE
     */
    int err = as_copy(curproc->p_addrspace, &new_proc->p_addrspace);
    if (err) {
        proc_destroy(new_proc);
        lock_release(curproc->p_mutex);
        return err;
    }

    /*
     * COPY PARENT'S LOCAL FILE TABLE
     */
    local_table_copy(curproc->local_file_table, new_proc->local_file_table);

    /*
     * COPY PARENT'S TRAPFRAME
     */
    new_tf = kmalloc(sizeof(struct trapframe));
    if (new_tf == NULL) {
        proc_destroy(new_proc);
        lock_release(curproc->p_mutex);
        return ENOMEM;
    }

    memcpy(new_tf, tf, sizeof(struct trapframe));

    result = thread_fork("user_proc_thread", new_proc, enter_forked_process, new_tf, (unsigned long) new_proc->pid);

    if (result) {
        lock_release(curproc->p_mutex);
        return result;
    }

    curproc->p_num_childs++;
    for (int i = 0; i < MAX_CHILDS; i++) {
        if (curproc->p_childs[i] == NULL) {
            curproc->p_childs[i] = new_proc;
            break;
        }
    }

    *pid = new_proc->pid;

    lock_release(curproc->p_mutex);
    return 0;
}

/*
 * Implementation of a helper process for sys_fork()
 */
void enter_forked_process(void *tf, unsigned long pid) {

    struct trapframe new_tf = *((struct trapframe *) tf);
    (void) pid;



    // Based on usage in the syscall method of syscall.c, these should work:
    // Register v0 in the trapframe is the syscall value OR the return value, in this case
    new_tf.tf_v0 = 0;

    // Apparently register a3 in the trapframe is used for error checking, so this indicates no error
    new_tf.tf_a3 = 0;

    // Increment the program counter so that we don't trap forever
    new_tf.tf_epc += 4;

    as_activate();
    mips_usermode(&new_tf);

    // This theoretically shouldn't be reachable
    KASSERT(1 == 2);
}