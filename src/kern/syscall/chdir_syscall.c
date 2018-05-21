#include <files.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <kern/stat.h> /* for getting the file size through VOP_STAT */
#include <copyinout.h> /* for using the copyinstr function */

#define BUF_SIZE 255 /* maximum valid length of filename */


/* chdir() syscall */

int sys_chdir(const_userptr_t pathName){
	char pathNameInput[BUF_SIZE];
	size_t actual;
	int err;

	if (pathName == NULL){
		return EFAULT;
	}

	if ((err =  copyinstr(pathName, pathNameInput, BUF_SIZE, &actual) != 0)){
		return err;
	}

	err = vfs_chdir(pathNameInput);
	return err;

}