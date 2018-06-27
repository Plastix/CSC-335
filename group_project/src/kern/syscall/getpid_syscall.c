#include <types.h>
#include <current.h>
#include <proc.h>
#include <synch.h>
#include <syscall.h>

int sys_getpid(pid_t *ret_pid) {
    *ret_pid = curproc->pid;
    return 0;
}