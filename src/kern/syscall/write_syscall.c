#include <types.h>
#include <syscall.h>

int sys_write(int filehandle, const_userptr_t buf, size_t size, size_t *ret) {
    (void) filehandle;
    (void) buf;
    (void) size;
    (void) ret;
    return 0;
}
