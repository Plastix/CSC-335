#include  <types.h>
#include <syscall.h>
#include <copyinout.h>
#include <lib.h>
#include <kern/errno.h>
#include <current.h>
#include <proc.h>

int sys_open(const_userptr_t filename, int flags, int *fd) {
    int err;  // Return value of internal OS161 calls

    if (filename == NULL || fd == NULL) { // Error: invalid pointers
        return EFAULT;
    }

    char *k_filename = kmalloc(MAX_FILENAME_SIZE_T);
    if (k_filename == NULL) {  // Error: creating internal filename buffer
        return EIO;
    }

    size_t len = sizeof(filename);
    err = copyinstr(filename, k_filename, MAX_FILENAME_SIZE_T, &len);
    if (err) { // Error: copying filename to kernel
        kfree(k_filename);
        return err;
    }

    File *new_file;
    // Atomic operation
    err = global_table_open_file(k_filename, flags, &new_file);
    if (err) {
        return err;
    }

    // Atomic operation
    err = local_table_add_file(curproc->local_file_table, new_file, fd);
    if (err) {
        return err;
    }

    return 0;

}
