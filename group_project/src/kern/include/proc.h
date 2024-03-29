/*
 * Copyright (c) 2013
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

#ifndef _PROC_H_
#define _PROC_H_

/*
 * Definition of a process.
 *
 * Note: curproc is defined by <current.h>.
 */

#include <spinlock.h>
#include <filetable.h>

struct addrspace;
struct thread;
struct vnode;

// Global file table
// Initialized in proc_bootstrap()
Global_File_Table *global_file_table;
// Global STD file descriptors
File_Desc *stdin_fd;
File_Desc *stdout_fd;
File_Desc *stderr_fd;

#define MAX_PROCS 128
#define MAX_CHILDS 32

typedef struct {
    struct proc *pcb;
    bool termed;
    bool p_termed;
    int exitcode;
    struct cv *waiting;

} pt_entry;

pt_entry *Global_Proc_Table[MAX_PROCS];
struct lock *GPT_lock;
unsigned GLOBAL_PROC_COUNT;

/*
 * Process structure.
 *
 * Note that we only count the number of threads in each process.
 * (And, unless you implement multithreaded user processes, this
 * number will not exceed 1 except in kproc.) If you want to know
 * exactly which threads are in the process, e.g. for debugging, add
 * an array and a sleeplock to protect it. (You can't use a spinlock
 * to protect an array because arrays need to be able to call
 * kmalloc.)
 *
 * You will most likely be adding stuff to this structure, so you may
 * find you need a sleeplock in here for other reasons as well.
 * However, note that p_addrspace must be protected by a spinlock:
 * thread_switch needs to be able to fetch the current address space
 * without sleeping.
 */
struct proc {
    char *p_name;            /* Name of this process */
    struct spinlock p_lock;        /* Lock for this structure */
    unsigned p_numthreads;        /* Number of threads in this process */

    /* VM */
    struct addrspace *p_addrspace;    /* virtual address space */

    /* VFS */
    struct vnode *p_cwd;        /* current working directory */

    // Process ID
    pid_t pid;

    // Return value of process
    int return_value;

    // Thread of proccess (1-1 mapping)
    struct thread *p_thread;

    // (Sleep) Lock for this structure
    struct lock *p_mutex;

    // Pointer to Parent
    struct proc *p_parent;

    // Child Data
    unsigned p_num_childs;

    struct proc *p_childs[MAX_CHILDS];

    Local_File_Table *local_file_table;
};

/* This is the process structure for the kernel and for kernel-only threads. */
extern struct proc *kproc;

/* Call once during system startup to allocate data structures. */
void proc_bootstrap(void);

/* Call once at the end of system start */
void file_table_bootstrap(void);

struct proc *proc_create(const char *name);

/* Create a fresh process for use by runprogram(). */
struct proc *proc_create_runprogram(const char *name);

/* Destroy a process. */
void proc_destroy(struct proc *proc);

/* Attach a thread to a process. Must not already have a process. */
int proc_addthread(struct proc *proc, struct thread *t);

/* Detach a thread from its process. */
void proc_remthread(struct thread *t);

/* Fetch the address space of the current process. */
struct addrspace *proc_getas(void);

/* Change the address space of the current process, and return the old one. */
struct addrspace *proc_setas(struct addrspace *);


#endif /* _PROC_H_ */
