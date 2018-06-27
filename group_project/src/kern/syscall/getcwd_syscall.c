#include <filetable.h> /*this adds the function prototypes for this file */
#include <synch.h>
#include <current.h>
#include <syscall.h>
#include <uio.h>
#include <vfs.h>
#include <kern/errno.h>
#include <proc.h>

/* __getcwd() syscall */
int sys____getcwd(userptr_t buf, size_t buflen, int *retval) {

    struct iovec iov;
    struct uio useruio;
    int err;

    lock_acquire(curproc->p_mutex);

    if (buf == NULL) {
        lock_release(curproc->p_mutex);
        return EFAULT;
    }

    if (buflen == 0) {
        lock_release(curproc->p_mutex);
        return EINVAL;
    }

    uio_kinit(&iov, &useruio, buf, buflen, 0, UIO_READ);

    useruio.uio_space = curproc->p_addrspace;
    useruio.uio_segflg = UIO_USERISPACE;

    err = vfs_getcwd(&useruio);

    if (err) {
        lock_release(curproc->p_mutex);
        return err;
    }

    *retval = buflen - useruio.uio_resid;

    lock_release(curproc->p_mutex);

    return 0;

}