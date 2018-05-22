#include <types.h>
#include <current.h>
#include <proc.h>
#include <synch.h>
#include <syscall.h>

int sys_getpid(pid_t *ret_pid) {
    lock_acquire(curproc->p_mutex);
    *ret_pid = curproc->pid;
    lock_release(curproc->p_mutex);
    return 0;
}