#include <types.h>
#include <copyinout.h>
#include <kern/errno.h>
#include <proc.h>
#include <synch.h>
#include <current.h>
#include <syscall.h>

int sys_waitpid(pid_t *ret_pid, int target_pid, const_userptr_t ret_status, int opts) {
    int tgt_pid = target_pid;
    int k_opts = opts;
    pt_entry *tgt_entry;
    int err;

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

    lock_acquire(GPT_lock);

    /*
    * GET TARGET PROC FROM GLOBAL TABLE
    */
    tgt_entry = Global_Proc_Table[tgt_pid];
    if (tgt_entry == NULL) {
        lock_release(GPT_lock);
        return ESRCH;
    }

    /*
     * CHECK TARGET PROC IS CHILD OF CALLING PROC
     */
    bool found = false;
    for (unsigned i = 0; i < MAX_CHILDS; i++) {
        if (curproc->p_childs[i] != NULL && curproc->p_childs[i]->pid == tgt_pid) {
            found = true;
            break;
        }
    }
    if (!found) {
        lock_release(GPT_lock);
        return ECHILD;
    }


    if (!tgt_entry->termed) {
        cv_wait(tgt_entry->waiting, GPT_lock);
    }

    KASSERT(tgt_entry->termed);

    /*
     * THE FOLLOWING IS ONLY REACHED WHEN THE TARGET PROC DIES
     */

    // SET RETURN PID
    *ret_pid = tgt_pid;

    // GET TARGET RETURN STATUS
    if (ret_status != NULL) {
        err = copyout(&tgt_entry->exitcode, (userptr_t) ret_status, sizeof(int));
        if (err) {
            lock_release(GPT_lock);
            return err;
        }
    }

    Global_Proc_Table[tgt_pid] = NULL;

    lock_release(GPT_lock);

    return 0;
}