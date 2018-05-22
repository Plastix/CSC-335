#include <files.h> /*this adds the function prototypes for this file */
#include <kern/errno.h>	/*for EINVAL, other errors */
#include <types.h>  /* for types like off_t, size_t, etc*/
#include <vnode.h>  /*for all the VOP methods */
#include <copyinout.h> /* for using the copyinstr function */
#include <kern/fcntl.h> /* for the O_RDONLY and the other flags */
#include <current.h> /* for the curthread variable */
#include <vfs.h> /* for vfs_open, close , etc */



/* close() system call */
/*int sys_close(int fd) {  //TODO: correct use of file descriptor?

    if (fd >= MAX_OPEN_FILES || fd < 0) {
        return EBADF;
    }

    if (curthread->File_Table[fd] == NULL) {   // TODO: where is the file table used?
        return EBADF;
    }
*/
    /* check if file handle at that position has an ref_count of 1*/
/*
    struct File *fdesc = curthread->File_Table[fd];

    fdesc->ref_count--;

    if (fdesc->ref_count == 0) {
        curthread->File_Table[fd] = NULL;
        //fd_destroy(fdesc);
    } else {
        curthread->File_Table[fd] = NULL;
    }
*/
    /*on success , return 0*/
/*    return 0;
}
*/