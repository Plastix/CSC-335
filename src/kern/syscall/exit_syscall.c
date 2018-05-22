#include <syscall.h>
#include <thread.h>
#include <synch.h>
#include <current.h>
#include <proc.h>

void sys__exit(int code) {
    (void) code;
    lock_acquire(curproc->p_mutex);

    curproc->return_value = code;

    local_table_close_all(curproc->local_file_table);

    // TODO Kill Children

    // Signal to all waiting processes
    cv_broadcast(curproc->waiting, curproc->p_mutex);

    lock_release(curproc->p_mutex);

    thread_exit();
}
