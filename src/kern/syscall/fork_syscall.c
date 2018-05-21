//
// Created by boggsj on 5/16/18.
//

#include <types.h>
#include <syscall.h>

pid_t sys_fork(void) {
    // TODO (James): Fill in fork()
    /*
     * What needs to be done (not necessarily in order):
     *  - Get currently running process
     *  - Clone process' address space into new struct proc
     *  - Create new thread for new struct proc
     *  - Start clone of original process running on thread at same mem addr
     *  - Return two different values depending on who's getting returned to (HOW???)
     */
    return (pid_t) -1; //TODO (James): Change this to return the proper PID
}