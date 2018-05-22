#include <types.h>
#include <lib.h>
#include <proc.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>

/* getpid() syscall */
int sys_getpid( int *retval ) {
	KASSERT( curthread != NULL );
	KASSERT( curthread->t_proc != NULL );

	spinlock_acquire( curthread->t_proc );
	*retval = curthread->t_proc->pid;
	spinlock_acquire( curthread->t_proc );

	return 0;
}