#include <filetable.h>
#include <kern/errno.h>
#include <kern/stat.h>
#include <syscall.h>
#include <current.h>
#include <kern/seek.h>
#include <synch.h>
#include <proc.h>
#include <vfs.h>


/* lseek() syscall
 *
 * On success, lseek returns the new position. On error, -1 is returned,
 * and errno is set according to the error encountered.
 * */
int sys_lseek(int fd, off_t pos, int whence, off_t *retVal) {

    int err;
    //off_t currentPosition;
    off_t endPosition;
    off_t newPos;
    struct stat st;

    *retVal = -1;

    lock_acquire(curproc->p_mutex);

    if (fd < 0 || fd >= MAX_LOCAL_TABLE_SIZE) {
        lock_release(curproc->p_mutex);
        return EBADF;
    }

    if (curproc->local_file_table == NULL) {
        lock_release(curproc->p_mutex);
        return EBADF;
    }

    File_Desc *fdesc = local_table_get(curproc->local_file_table, fd);

    if (fdesc == NULL) {
        lock_release(curproc->p_mutex);
        return EBADF;
    }

    /* synchronize the process */
    lock_acquire(fdesc->lk);


    err = VOP_ISSEEKABLE(fdesc->file->node);
    if (err) {
        lock_release(fdesc->lk);
        lock_release(curproc->p_mutex);
        return err;
    }


    switch (whence) {
        case SEEK_SET:
        /*    if (pos < 0) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return EINVAL;    // seek position is negative
            }

            if ((err = VOP_ISSEEKABLE(fdesc->file->node)) != 0) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return ESPIPE;    // SEEK fails
            } */
            //fdesc->seek_location = pos;
            //*retVal = pos;
            newPos = pos;
            break;

        case SEEK_CUR:
            /* currentPosition = fdesc->seek_location;
            newPos = currentPosition + pos;

            if (newPos < 0) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return EINVAL;
            }

            if ((err = VOP_ISSEEKABLE(fdesc->file->node)) != 0) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return ESPIPE;
            } */

            //fdesc->seek_location = newPos;
            //*retVal = newPos;

            newPos = fdesc->seek_location + pos;
            break;

        case SEEK_END:
            endPosition = st.st_size;
            /*
            if (newPos < 0) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return EINVAL;
            }

            if ((err = VOP_ISSEEKABLE(fdesc->file->node)) != 0) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return ESPIPE;
            }
            */
            err = VOP_STAT(fdesc->file->node, &st);
            if (err) {
                lock_release(fdesc->lk);
                lock_release(curproc->p_mutex);
                return err;
            }

            newPos = endPosition + pos;
            //fdesc->seek_location = newPos; // any difference when pos is negative, zero, or positive ??
            //*retVal = newPos;

            break;

        default:
            lock_release(fdesc->lk);
            lock_release(curproc->p_mutex);
            return EINVAL;
    }

    if (newPos < 0) {
        lock_release(fdesc->lk);
        lock_release(curproc->p_mutex);
        return EINVAL;    // seek position is negative
    }

    fdesc->seek_location = newPos;
    *retVal = newPos;

    lock_release(fdesc->lk);
    lock_release(curproc->p_mutex);

    return 0;

}
