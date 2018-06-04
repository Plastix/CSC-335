//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>
#include <copyinout.h>
#include <kern/errno.h>
#include <proc.h>
#include <synch.h>
#include <current.h>

int sys_waitpid(pid_t *ret_pid, userptr_t target_pid, userptr_t ret_status, userptr_t opts) {
    pid_t tgt_pid;
    int k_opts;
    pt_entry *tgt_entry;

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
    if (tgt_pid <= 0) {
        return ESRCH;
    }

    tgt_entry = Global_Proc_Table[tgt_pid];
    if (tgt_entry == NULL) {
        return ESRCH;
    }

    /*
     * CHECK TARGET PROC IS CHILD OF CALLING PROC
     */
    int found = 0;
    for (unsigned i = 0; i < MAX_CHILDS; i++) {
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

    if (!tgt_entry->termed) {
        cv_wait(tgt_entry->waiting, GPT_lock);
    }

    /*
     * THE FOLLOWING IS ONLY REACHED WHEN THE TARGET PROC DIES
     */

    // SET RETURN PID
    *ret_pid = tgt_pid;

    // GET TARGET RETURN STATUS
    if (ret_status != NULL) {
        copyout(&tgt_entry->exitcode, ret_status, sizeof(int));
    }

    Global_Proc_Table[tgt_pid] = NULL;

    lock_release(GPT_lock);

    return 0;
}