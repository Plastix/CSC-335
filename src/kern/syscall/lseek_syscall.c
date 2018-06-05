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
    off_t newPos; // New seek position
    struct stat st;

    File_Desc *fdesc = local_table_get(curproc->local_file_table, fd);
    if (fdesc == NULL) {
        return EBADF;
    }

    if (fdesc->standard != -1) {
        return ESPIPE;
    }

    lock_acquire(fdesc->lk);

    if (!VOP_ISSEEKABLE(fdesc->file->node)) {
        lock_release(fdesc->lk);
        return EINVAL;
    }

    switch (whence) {
        case SEEK_SET:
            newPos = pos;
            break;

        case SEEK_CUR:
            newPos = fdesc->seek_location + pos;
            break;

        case SEEK_END:
            // Get current file stats
            err = VOP_STAT(fdesc->file->node, &st);
            if (err) {
                lock_release(fdesc->lk);
                return err;
            }

            newPos = st.st_size + pos;
            break;

        default:
            lock_release(fdesc->lk);
            return EINVAL;
    }

    if (newPos < 0) {
        lock_release(fdesc->lk);
        return EINVAL;    // seek position is negative
    }

    fdesc->seek_location = newPos;
    *retVal = newPos;

    lock_release(fdesc->lk);

    return 0;

}
