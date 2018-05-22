#include <filetable.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <copyinout.h> /* for using the copyinstr function */
#include <kern/fcntl.h> /* for the O_RDONLY and the other flags */
#include <current.h> /* for the curthread variable */
#include <vfs.h> /* for vfs_open, close , etc */
#include <proc.h>


/* close() system call */
int sys_close(int fd) {

    if (fd >= MAX_LOCAL_TABLE_SIZE || fd < 0) {
        return EBADF;
    }

   // if (curthread->t_proc->local_file_table[fd] == NULL) {
   //     return EBADF;
   // }

    /* check if file handle at that position has an ref_count of 1*/




    /*on success , return 0*/
    return 0;
}
