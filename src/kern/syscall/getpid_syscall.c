#include <types.h>
#include <lib.h>
#include <proc.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>
#include <errno.h>
#include <synch.h>

/* getpid() syscall */
int sys_getpid( int *retval ) {

	lock_acquire(curproc->p_mutex);

	if (curthread == NULL) {
		return ENOTSUP;   /* Threads operation not supported */??
	}

	if (curthread->t_proc == NULL ) {
		return ESRCH;     /* No such process */ ??
	}

	*retval = curthread->t_proc->pid;

	lock_release(curproc->p_mutex);

	return 0;
}