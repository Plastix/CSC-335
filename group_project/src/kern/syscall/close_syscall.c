#include <filetable.h> /*this adds the function prototypes for this file */
#include <current.h> /* for the curthread variable */
#include <proc.h>
#include <syscall.h>
#include <kern/errno.h>


/* close() system call */
int sys_close(int fd) {

    if (fd < 0 || fd >= MAX_LOCAL_TABLE_SIZE) {
        return EBADF;
    }

    int err = local_table_close_file(curproc->local_file_table, fd);

    if (err) {
        return err;
    }
    
    return 0;
}
