//
// Created by boggsj on 5/16/18.
//

#include <sys/types.h>
#include <kern/unistd.h>

pid_t waitpid(pid_t pid, int *returncode, int flags) {
    // TODO (James): Fill in waitpid()
    /*
     * Things to be done:
     *  - Get pointer to struct proc referenced by PID from a global table
     *  - Call `wait()` on the target process' `proc->waiting` cv
     *  - Once the target process terminates, the `wait()` call will return
     *  - Get `proc->return_vale` from the target process and return it
     *
     */
}