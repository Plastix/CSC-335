#include <filetable.h> /*this adds the function prototypes for this file */
#include <current.h>
#include <proc.h>
#include <syscall.h>
#include <kern/errno.h>    /*for EINVAL, other errors */


/* dup2() syscall */
int sys_dup2(int oldfd, int newfd, int *retval) {

    if (oldfd >= MAX_LOCAL_TABLE_SIZE || oldfd < 0 || newfd >= MAX_LOCAL_TABLE_SIZE || newfd < 0) {
        return EBADF;
    }

    int err = local_table_dup2(curproc->local_file_table, oldfd, newfd);

    if (err) {
        return err;
    }

    *retval = newfd;

    return 0;
}
