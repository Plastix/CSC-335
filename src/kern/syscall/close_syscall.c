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
    File_Desc *fdesc = local_table_get(curproc->local_file_table, fd);

    /* check if file handle at that position has an ref_count of 1*/




    /*on success , return 0*/
    return 0;
}
