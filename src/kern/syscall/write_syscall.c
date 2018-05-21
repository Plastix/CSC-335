#include <types.h>
#include <syscall.h>

ssize_t sys_write(int filehandle, const_userptr_t buf, size_t size) {
    (void) filehandle;
    (void) buf;
    (void) size;
    return 0;
}
