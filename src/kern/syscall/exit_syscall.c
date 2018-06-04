#include <syscall.h>
#include <thread.h>
#include <synch.h>
#include <current.h>
#include <proc.h>

void sys__exit(int code) {
    lock_acquire(GPT_lock);

    pt_entry *current = Global_Proc_Table[curproc->pid];

    // Proc table invariant invalid
    KASSERT(current != NULL);

    current->termed = true;
    current->exitcode = code;

    for (int i = 0; i < MAX_CHILDS; ++i) {
        struct proc *child_pcb = curproc->p_childs[i];
        if (child_pcb != NULL) {
            pt_entry *child = Global_Proc_Table[child_pcb->pid];
            KASSERT(child != NULL);
            child->p_termed = true;
        }
    }

    cv_broadcast(current->waiting, GPT_lock);

    if (current->p_termed) {
        Global_Proc_Table[curproc->pid] = NULL;
    }

    lock_release(GPT_lock);

    proc_destroy(current->pcb);
    thread_exit();
}
