#include <types.h>
#include <syscall.h>

ssize_t sys_read(int filehandle, userptr_t buf, size_t size) {
    (void) filehandle;
    (void) buf;
    (void) size;
    return 0;
}