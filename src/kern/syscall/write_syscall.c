#include <types.h>
#include <current.h>
#include <kern/errno.h>
#include <synch.h>
#include <kern/fcntl.h>
#include <vfs.h>
#include <uio.h>
#include <copyinout.h>
#include <proc.h>
#include <syscall.h>
#include <kern/unistd.h>

static int write_to_disk(File_Desc *desc, const_userptr_t buf, size_t size, size_t *ret) {
    // Acquire the lock on the descriptor
    lock_acquire(desc->lk);

    if (desc->flags != O_WRONLY || desc->flags != O_RDWR) {
        *ret = (size_t) -1;
        lock_release(desc->lk);
        return EBADF;
    }

    File *file = desc->file;

    char *k_buffer = kmalloc(size);
    if (k_buffer == NULL) {
        *ret = (size_t) -1;
        lock_release(desc->lk);
        return ENOMEM;
    }

    struct iovec iov;
    struct uio ku;
    int err;

    // Acquire the read/write lock protecting the vnode
    lock_acquire(file->lk);
    err = copyin(buf, k_buffer, size);
    if (err) {
        *ret = (size_t) -1;
        // Release both locks, must be in this order
        lock_release(file->lk);
        lock_release(desc->lk);
        return err;
    }

    uio_kinit(&iov, &ku, k_buffer, size, desc->seek_location, UIO_READ);
    err = VOP_WRITE(file->node, &ku);

    // Reading file failed somehow
    if (err) {
        *ret = (size_t) -1;
        // Release both locks, must be in this order
        lock_release(file->lk);
        lock_release(desc->lk);
        return err;
    }

    size_t bytes_copied = size - ku.uio_resid;

    // Update seek location
    desc->seek_location += bytes_copied;
    // Return amount of bytes copied
    *ret = bytes_copied;

    // Release both locks, must be in this order
    lock_release(file->lk);
    lock_release(desc->lk);

    return 0;
}

static int write_std(File_Desc *desc, const_userptr_t buf, size_t size, size_t *ret) {
    lock_acquire(desc->lk);
    File *f = desc->file;
    lock_acquire(f->lk);

    char *bytes = kmalloc(size);
    if (bytes == NULL) {
        *ret = (size_t) -1;
        lock_release(f->lk);
        lock_release(desc->lk);
        return ENOMEM;
    }

    int err = copyin(buf, bytes, size);
    if (err) {
        *ret = (size_t) -1;
        lock_release(f->lk);
        lock_release(desc->lk);
        return err;
    }

    size_t read = 0;
    while (read < size) {
        putch(bytes[read]);
        read++;
    }

    *ret = read;

    lock_release(f->lk);
    lock_release(desc->lk);

    return 0;

}

int sys_write(int filehandle, const_userptr_t buf, size_t size, size_t *ret) {
    if (buf == NULL) {
        return EFAULT;
    }

    Local_File_Table *file_table = curproc->local_file_table;

    // Get the file descriptor object from the local file table
    File_Desc *desc = local_table_get(file_table, filehandle);
    if (desc == NULL) {
        return EBADF;
    }

    int standard = desc->standard;
    switch (standard) {
        case STDERR_FILENO:
            // Intentional pass-through
        case STDOUT_FILENO:
            return write_std(desc, buf, size, ret);
        case -1:
            return write_to_disk(desc, buf, size, ret);
        default:
            return EBADF;
    }
}
