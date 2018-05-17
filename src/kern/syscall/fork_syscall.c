//
// Created by boggsj on 5/16/18.
//


#include <sys/types.h>
#include <kern/unistd.h>

pid_t fork(void) {
    // TODO (James): Fill in fork()
    /*
     * What needs to be done (not necessarily in order):
     *  - Get currently running process
     *  - Clone process' address space into new struct proc
     *  - Create new thread for new struct proc
     *  - Start clone of original process running on thread at same mem addr
     *  - Return two different values depending on who's getting returned to (HOW???)
     */
}