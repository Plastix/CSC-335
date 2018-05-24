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

int sys_mkdir(const_userptr_t pathName, mode_t mode, int *retVal) {

    (void) pathName;
    (void) mode;
    *retVal = 0;

    return 0;

}