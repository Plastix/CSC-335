#include <filetable.h>
#include <kern/errno.h>
#include <kern/stat.h>
#include <syscall.h>
#include <current.h>
#include <kern/seek.h>
#include <synch.h>
#include <proc.h>
#include <vfs.h>


/* lseek() syscall */

int sys_lseek(int fd, off_t pos, int whence, off_t *retVal) {

    int err;
    off_t currentPosition;
    off_t endPosition;
    off_t posUpdate;
    struct stat st;

    lock_acquire(curproc->p_mutex);

    File_Desc *fdesc = local_table_get(curproc->local_file_table, fd);

    if (fdesc == NULL) {
        return EBADF;
    }

    /* synchronize the process */
    lock_acquire(fdesc->lk);

    switch (whence) {    // logic for different cases
        case SEEK_SET:
            if (pos < 0) {
                lock_release(fdesc->lk);
                return EINVAL;    // seek position is negative
            }

            posUpdate = pos;
            if ((err = VOP_ISSEEKABLE(fdesc->file->node)) != 0) {
                lock_release(fdesc->lk);
                return err;    // SEEK fails
            }
            fdesc->seek_location = posUpdate;
            *retVal = fdesc->seek_location;
            break;

        case SEEK_CUR:
            currentPosition = fdesc->seek_location;
            posUpdate = currentPosition + pos;

            if (posUpdate < 0) {  // ?? Any up boundary ??
                lock_release(fdesc->lk);
                return EINVAL;  /* Invalid argument */
            }

            if ((err = VOP_ISSEEKABLE(fdesc->file->node)) != 0) {
                lock_release(fdesc->lk);
                return err;
            }
            fdesc->seek_location = posUpdate;
            *retVal = fdesc->seek_location;
            break;

        case SEEK_END:
            if ((err = VOP_STAT(fdesc->file->node, &st)) != 0) {
                lock_release(fdesc->lk);
                return err;
            }

            endPosition = st.st_size;
            posUpdate = endPosition + pos;

            if (posUpdate < 0) {
                lock_release(fdesc->lk);
                return EINVAL;
            }

            if ((err = VOP_ISSEEKABLE(fdesc->file->node)) != 0) {
                lock_release(fdesc->lk);
                return err;
            }

            fdesc->seek_location = posUpdate; // any difference when pos is negative, zero, or positive ??

            *retVal = fdesc->seek_location;
            break;

        default:
            return EINVAL;
    }

    lock_release(fdesc->lk);

    lock_release(curproc->p_mutex);

    return 0;

}
