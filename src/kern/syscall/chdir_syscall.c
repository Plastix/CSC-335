#include <filetable.h>
#include <kern/errno.h>
#include <copyinout.h>
#include <vfs.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <synch.h>


/* chdir() syscall */

int sys_chdir(const_userptr_t pathName){
	lock_acquire(curthread->t_proc->p_mutex);

	char pathNameInput[MAX_FILENAME_LEN];
	size_t actual;
	int err;

	if (pathName == NULL){
		return EFAULT;
	}

	if ((err =  copyinstr(pathName, pathNameInput, MAX_FILENAME_LEN, &actual) != 0)){
		return err;
	}

	err = vfs_chdir(pathNameInput);

	lock_release(curthread->t_proc->p_mutex);
	return err;

}