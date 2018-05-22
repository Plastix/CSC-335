#include <files.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <lib.h>  /* for kmalloc, fuctions etc */
#include <kern/stat.h> /* for getting the file size through VOP_STAT */
#include <copyinout.h> /* for using the copyinstr function */
#include <kern/fcntl.h> /* for the O_RDONLY and the other flags */
#include <kern/limits.h>  /* for the OPEN_MAX - allows 128 open files constant */
#include <current.h> /* for the curthread variable */
#include <vfs.h> /* for vfs_open, close , etc */
#include <uio.h> /* for uio and iovec , this is for moving data from kernel to userspace and vice versa */
#include <kern/seek.h> /* for seek operation lseek() */
#include <kern/unistd.h> /* for console flags STDIN, STDOUT and STDERR */


#define BUF_SIZE 255 /* maximum valid length of filename */

/* __getcwd() syscall */
int sys_getcwd(userptr_t buf, int * retval){

    struct uio user;
    struct iovec iov;
    uio_kinit(&iov, &user, buf, BUF_SIZE, 0, UIO_READ);
    user.uio_segflg = UIO_USERSPACE;
//    user.uio_space = curthread->t_addrspace;

    if (buf == NULL) {
        return EFAULT;
    }

    int err;
    if ((err = vfs_getcwd(&user) != 0)){
        return err;
    }

    *retval = BUF_SIZE - user.uio_resid;
    return 0;

}