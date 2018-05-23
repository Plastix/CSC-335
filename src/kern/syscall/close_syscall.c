#include <filetable.h> /*this adds the function prototypes for this file */
#include <current.h> /* for the curthread variable */
#include <proc.h>
#include <syscall.h>
#include <kern/errno.h>


/* close() system call */
int sys_close(int fd) {

    /* check if file handle at that position has an ref_count of 1*/

    if (fd < 0 || fd >= MAX_LOCAL_TABLE_SIZE) {
        return EBADF;
    }

    int err = local_table_close_file(curproc->local_file_table, fd);

    if (err) {
        return err;
    }

    /*on success , return 0*/
    return 0;
}
