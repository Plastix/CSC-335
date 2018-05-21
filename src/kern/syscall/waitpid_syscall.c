//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>

pid_t sys_waitpid(pid_t pid, int *returncode, int flags) {
    // TODO (James): Fill in waitpid()
    /*
     * Things to be done:
     *  - Get pointer to struct proc referenced by PID from a global table
     *  - Call `wait()` on the target process' `proc->waiting` cv
     *  - Once the target process terminates, the `wait()` call will return
     *  - Get `proc->return_vale` from the target process and return it
     *
     */

    (void) returncode;
    (void) flags;
    return pid;
}