#include <filetable.h> /*this adds the function prototypes for this file */
#include <syscall.h>
#include <uio.h>
#include <vfs.h>

/* __getcwd() syscall */
int sys____getcwd(userptr_t buf, size_t buflen, int *retval) {

    struct iovec iov;
    struct uio useruio;
    int err;

    uio_kinit(&iov, &useruio, buf, buflen, 0, UIO_READ);

    err = vfs_getcwd(&useruio);
    if (err) {
        return err;
    }

    *retval = buflen - useruio.uio_resid;

    return 0;

}