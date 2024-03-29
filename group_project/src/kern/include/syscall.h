/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>
#include <cdefs.h> /* for __DEAD */

struct trapframe; /* from <machine/trapframe.h> */

#define MAX_FILENAME_LEN 128
#define MAX_FILENAME_SIZE_T (sizeof(char) * MAX_FILENAME_LEN)

/*
 * The system call dispatcher.
 */

void syscall(struct trapframe *tf);

/*
 * Support functions.
 */

/* Helper for fork(). You write this. */
void enter_forked_process(void *tf, unsigned long pid);

/* Enter user mode. Does not return. */
__DEAD void enter_new_process(int argc, userptr_t argv, userptr_t env,
                              vaddr_t stackptr, vaddr_t entrypoint);


/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

int sys___time(userptr_t user_seconds, userptr_t user_nanoseconds);

void sys__exit(int code);

int sys_open(const_userptr_t filename, int flags, int *fd);

int sys_read(int filehandle, userptr_t buf, size_t size, size_t *ret);

int sys_write(int filehandle, const_userptr_t buf, size_t size, size_t *ret);

int sys_fork(struct trapframe *tf, pid_t *pid);

int sys_execv(const_userptr_t progname, const_userptr_t args);

int sys_waitpid(pid_t *ret_pid, int target_pid, const_userptr_t ret_status, int opts);

int sys_getpid(pid_t *ret_pid);


int sys_close(int fd);

int sys_dup2(int oldfd, int newfd, int *retVal);

int sys_lseek(int fd, off_t pos, int whence, off_t *retVal);

int sys_chdir(const_userptr_t pathname, int *retVal);

int sys____getcwd(userptr_t buf, size_t buflen, int *retval);


// for testing purpose
int sys_mkdir(const_userptr_t pathname, mode_t mode, int *retVal);

#endif /* _SYSCALL_H_ */
