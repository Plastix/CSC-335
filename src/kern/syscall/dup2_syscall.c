#include <filetable.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <current.h> /* for the curthread variable */
#include <kern/unistd.h> /* for console flags STDIN, STDOUT and STDERR */


/* dup2() syscall */
int sys_dup2(int oldfd, int newfd, int * retval) {

    if (oldfd >= MAX_LOCAL_TABLE_SIZE || oldfd < 0 || newfd >= MAX_LOCAL_TABLE_SIZE || newfd < 0 ){
        return EBADF;
    }


    return 0;
}
