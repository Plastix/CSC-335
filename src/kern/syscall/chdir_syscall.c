#include <filetable.h>
#include <kern/errno.h>
#include <copyinout.h>
#include <vfs.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <synch.h>


/* chdir() syscall
 *
 * On success, chdir returns 0. On error, -1 is returned,
 * and errno is set according to the error encountered.
 * */

int sys_chdir(const_userptr_t pathName, int *retVal) {

    char pathNameInput[MAX_FILENAME_LEN];
    size_t actual;
    int err;

    lock_acquire(curproc->p_mutex);

    if (pathName == NULL) {
        *retVal = -1;
        lock_release(curproc->p_mutex);
        return EFAULT;
    }

    if ((err = copyinstr(pathName, pathNameInput, MAX_FILENAME_LEN, &actual) != 0)) {
        *retVal = -1;
        lock_release(curproc->p_mutex);
        return err;
    }

    if ((err = vfs_chdir(pathNameInput) != 0)) {
        *retVal = -1;
        lock_release(curproc->p_mutex);
        return err;
    }

    *retVal = 0;
    lock_release(curproc->p_mutex);

    return 0;

}