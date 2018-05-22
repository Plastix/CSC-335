#include <syscall.h>
#include <thread.h>

void sys__exit(int code) {
    (void) code;
    thread_exit();
}
