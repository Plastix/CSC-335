//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>

int sys_execv(const char *prog, char *const *args) {
    // TODO (James): Fill in execv()
    /*
     * What needs to be done:
     *  - Get the binary file from the VFS given the string prog parameter
     *  - Load the binary into VM
     *  - Overwrite current process' program code with binary (part of above?)
     *  - Either wipe or keep other aspects of addr space (file handles, data, etc.)
     *  - Return if there's an error
     */
    (void) prog;
    (void) args;
    return 0;
}