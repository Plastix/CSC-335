#include <files.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <lib.h>  /* for kmalloc, fuctions etc */
#include <kern/stat.h> /* for getting the file size through VOP_STAT */
#include <current.h> /* for the curthread variable */
#include <kern/seek.h> /* for seek operation lseek() */
#include <synch.h>


/* lseek() call handler */
/*
int sys_lseek(int fd, off_t pos, int whence, off_t * retVal64){

    int err;
    off_t currentPosition;
    off_t endPosition;
    off_t posSeek;
    struct stat buffer;
*/
    /* Step 1: Check if the file descriptor passed in is valid */
/*    if (fd >= MAX_OPEN_FILES || fd < 0) {	// fd is out of bounds of the file table
        return EBADF;
    }

    struct fhandle * fdesc = curthread->t_fdtable[fd];
    if (fdesc == NULL) {
        return EBADF;
    }
*/
    /* Adding locks to synchronize the whole process */
/*    lock_acquire(fdesc->lock);
    switch(whence) {	// logic for different cases
        case SEEK_SET: //VOP_TRYSEEK(vnode, position)
            if (pos < 0) {
                lock_release(fdesc->lock);
                return EINVAL;	// seek position is negative
            }

            posSeek = pos;
            if ((err = VOP_TRYSEEK(fdesc->vn, posSeek)) != 0) {
                lock_release(fdesc->lock);
                return ESPIPE;	// in case the SEEK fails
            }
            fdesc->offset = posSeek;
            *retVal64 = fdesc->offset;
            break;

        case SEEK_CUR:  currentPosition = fdesc->offset;
            posSeek = currentPosition + pos;

            if (posSeek < 0) {
                lock_release(fdesc->lock);
                return EINVAL;
            }

            if ((err = VOP_TRYSEEK(fdesc->vn, posSeek)) != 0) {
                lock_release(fdesc->lock);
                return ESPIPE;
            }
            fdesc->offset = posSeek;
            *retVal64 = fdesc->offset;
            break;

        case SEEK_END:  VOP_STAT(fdesc->vn, &buffer);
            endPosition = buffer.st_size;
            posSeek = endPosition + pos;

            if (posSeek < 0) {
                lock_release(fdesc->lock);
                return EINVAL;
            }

            if ((err = VOP_TRYSEEK(fdesc->vn, posSeek)) != 0) {
                lock_release(fdesc->lock);
                return ESPIPE;
            }

            if (pos < 0) {
                fdesc->offset = posSeek + pos;
            } else {
                fdesc->offset = posSeek;
            }

            *retVal64 = fdesc->offset;
            break;
        default:
            return EINVAL;
    }
    lock_release(fdesc->lock);

    return 0;

}
*/