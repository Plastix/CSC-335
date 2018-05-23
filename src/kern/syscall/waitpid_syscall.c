//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>
#include <lib.h>
#include <copyinout.h>
#include <kern/errno.h>
#include <proc.h>
#include <synch.h>
#include <current.h>

int sys_waitpid(pid_t *ret_pid, userptr_t target_pid, userptr_t ret_status, userptr_t opts) {
    /*
     * First, ensure paramters are not null
     */
    KASSERT(ret_pid != NULL);
    KASSERT(target_pid != NULL);

    pid_t tgt_pid;
    int k_opts;
    struct proc *target_proc;

    /*
     * CAST PARAMS TO PROPER TYPE
     */
    copyin(target_pid, &tgt_pid, sizeof(pid_t));
    copyin(opts, &k_opts, sizeof(int));

    /*
     * CHECK OPTS TO ENSURE IT IS 0
     *  - No other opts are implemented
     */
    if (k_opts) {
        return EINVAL;
    }

    /*
     * CHECK TARGET PROC IS VALID
     */
    if (Global_Proc_Table[tgt_pid] == NULL) {
        return ESRCH;
    }

    /*
     * CHECK TARGET PROC IS CHILD OF CALLING PROC
     */
    int found = 0;
    for (unsigned i=0; i<curproc->p_num_childs; i++) {
        if (curproc->p_childs[i] != NULL && curproc->p_childs[i]->pid == tgt_pid) {
            found++;
        }
    }
    if (!found) {
        return ECHILD;
    }

    /*
     * GET TARGET PROC FROM GLOBAL TABLE
     */
    lock_acquire(GPT_lock);
    target_proc = Global_Proc_Table[tgt_pid];
    lock_release(GPT_lock);

    /*
     * WAIT ON TARGET PROC WAITING CV
     */
    lock_acquire(target_proc->p_mutex);
    cv_wait(target_proc->waiting, target_proc->p_mutex);
    lock_release(target_proc->p_mutex);
    /*
     * THE FOLLOWING IS ONLY REACHED WHEN THE TARGET PROC DIES
     */

    // SET RETURN PID
    *ret_pid = tgt_pid;
    // GET TARGET RETURN STATUS
    if (ret_status != NULL) {
        copyout(&target_proc->return_value, ret_status, sizeof(int));
    }

    /*
     * Things to be done:
     *  - Get pointer to struct proc referenced by PID from a global table
     *  - Call `wait()` on the target process' `proc->waiting` cv
     *  - Once the target process terminates, the `wait()` call will return
     *  - Get `proc->return_vale` from the target process and return it
     *
     */
    return 0;
}