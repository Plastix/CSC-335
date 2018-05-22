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

static int read_from_disk(File_Desc *desc, userptr_t buf, size_t size, size_t *ret) {
    // Acquire the lock on the descriptor
    lock_acquire(desc->lk);

    if (desc->flags != O_RDONLY || desc->flags != O_RDWR) {
        lock_release(desc->lk);
        return EBADF;
    }

    File *file = desc->file;

    char *k_buffer = kmalloc(size);
    if (k_buffer == NULL) {
        lock_release(desc->lk);
        return ENOMEM;
    }

    struct iovec iov;
    struct uio ku;
    int err;

    // Acquire the readls/write lock protecting the vnode
    lock_acquire(file->lk);

    uio_kinit(&iov, &ku, k_buffer, size, desc->seek_location, UIO_READ);
    err = VOP_READ(file->node, &ku);

    // Reading file failed somehow
    if (err) {
        *ret = (size_t) -1;

        // Release both locks, must be in this order
        lock_release(file->lk);
        lock_release(desc->lk);
        return EIO;
    }

    size_t bytes_copied = size - ku.uio_resid;

    err = copyout(k_buffer, buf, bytes_copied);
    if (err) {
        *ret = (size_t) -1;

        // Release both locks, must be in this order
        lock_release(file->lk);
        lock_release(desc->lk);
        return EIO;
    }

    // Update seek location
    desc->seek_location += bytes_copied;
    // Return amount of bytes copied
    *ret = bytes_copied;

    // Release both locks, must be in this order
    lock_release(file->lk);
    lock_release(desc->lk);

    return 0;
}

static int read_stdin(File_Desc *desc, userptr_t buf, size_t size, size_t *ret) {
    lock_acquire(desc->lk);
    File *f = desc->file;
    lock_acquire(f->lk);

    char *bytes = kmalloc(size);
    if (bytes == NULL) {
        lock_release(f->lk);
        lock_release(desc->lk);
        return ENOMEM;
    }

    size_t read = 0;

    while (read < size) {
        char in = (char) getch();

        if (in == '\n' || in == '\r') {
            bytes[read] = '\n';
            read++;
            putch('\n');
            break;
        } else {
            bytes[read] = in;
            read++;
        }
    }

    int err = copyout(bytes, buf, read);
    if (err) {
        lock_release(f->lk);
        lock_release(desc->lk);
        return err;
    }

    *ret = read;

    lock_release(f->lk);
    lock_release(desc->lk);

    return 0;

}

int sys_read(int filehandle, userptr_t buf, size_t size, size_t *ret) {
    Local_File_Table *file_table = curproc->local_file_table;

    // Get the file descriptor object from the local file table
    File_Desc *desc = local_table_get(file_table, filehandle);
    if (desc == NULL) {
        return EBADF;
    }

    int standard = desc->standard;
    switch (standard) {
        case STDIN_FILENO:
            return read_stdin(desc, buf, size, ret);
        case -1:
            return read_from_disk(desc, buf, size, ret);
        default:
            return EBADF;
    }


}