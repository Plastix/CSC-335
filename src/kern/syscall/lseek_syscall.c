#include <filetable.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <lib.h>  /* for kmalloc, fuctions etc */
#include <kern/stat.h> /* for getting the file size through VOP_STAT */
#include <current.h> /* for the curthread variable */
#include <kern/seek.h> /* for seek operation lseek() */
#include <synch.h>
#include <proc.h>
#include <vfs.h>

/* lseek() syscall */

int sys_lseek(int fd, off_t pos, int whence, off_t * retVal){

    int err;
    off_t currentPosition;
    off_t endPosition;
    off_t posUpdate;

    /* check if the file descriptor passed in is valid */
    if (fd >= MAX_LOCAL_TABLE_SIZE || fd < 0) {	// fd is out of bounds of the file table
        return EBADF;
    }

    struct File fdesc = curthread->t_proc->local_file_table[fd];

    if (fdesc == NULL) {
        return EBADF;
    }

    /* synchronize the process */
    lock_acquire(fdesc->lock);
    switch(whence) {	// logic for different cases
        case SEEK_SET:
            if (pos < 0) {
                lock_release(fdesc->lock);
                return EINVAL;	// seek position is negative
            }

            posUpdate = pos;
            if ((err = VOP_ISSEEKABLE(fdesc->vn)) != 0) {
                lock_release(fdesc->lock);
                return ESPIPE;	// SEEK fails
            }
            fdesc->offset = posUpdate;
            *retVal = fdesc->offset;
            break;

        case SEEK_CUR:
            currentPosition = fdesc->offset;
            posUpdate = currentPosition + pos;

            if (posUpdate < 0) {  // ?? Any up boundary ??
                lock_release(fdesc->lock);
                return EINVAL;
            }

            if ((err = VOP_ISSEEKABLE(fdesc->vn)) != 0) {
                lock_release(fdesc->lock);
                return ESPIPE;
            }
            fdesc->offset = posUpdate;
            *retVal = fdesc->offset;
            break;

        case SEEK_END:
            endPosition = 0; // TODO (Violet): the position of end-of-file ??
            posUpdate = endPosition + pos;

            if (posUpdate < 0) {
                lock_release(fdesc->lock);
                return EINVAL;
            }

            if ((err = VOP_ISSEEKABLE(fdesc->vn)) != 0) {
                lock_release(fdesc->lock);
                return ESPIPE;
            }

            fdesc->offset = posUpdate; // any difference when pos is negative, zero, or positive ??

            *retVal = fdesc->offset;
            break;

        default:
            return EINVAL;
    }
    lock_release(fdesc->lock);

    return 0;

}
