#include <files.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <current.h> /* for the curthread variable */
#include <kern/unistd.h> /* for console flags STDIN, STDOUT and STDERR */


/* dup2() syscall */
int sys_dup2(int oldfd, int newfd, int * retval) {

    if (oldfd >= MAX_OPEN_FILES || oldfd < 0 || newfd >= MAX_OPEN_FILES || newfd < 0 ){
        return EBADF;
    }

    /* checking if there is an oldfd that is already opened */
    if (curthread->File_Table[oldfd] == NULL) {
        return EBADF;
    }

    struct File *fdesc;
    fdesc = curthread->File_Table[oldfd];

    /* Check if newfd points to null*/
    if (curthread->File_Table[newfd] == NULL){
        curthread->File_Table[newfd] = fdesc;
        fdesc->ref_count++;
    }
    else {
        /* Close existing fd */
        //sys_close(newfd);
        curthread->File_Table[newfd] = fdesc;
        fdesc->ref_count++;
    }

    *retval = newfd;
    return 0;
}
