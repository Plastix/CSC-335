# Project 3 Design Document
James, Violet, Aidan (os161-bgp)

# Design Choices
What are the major components that you will have to add to OS/161 and how will they interact with OS/161?  To this end you may want to discuss your answers to the following questions.

1) Will there be one or many kernel threads?  Preemptive or non-preemptive.

We intend to have multiple kernel threads, with each user process thread mapping to one kernel thread in the one-to-one approach highlighted in the book. Since we, at the moment, do not intend to implement user process multi-threading, there will be a one-to-one mapping between kernel threads and user processes as well as user threads, since each process will only have one thread. Kernel threads will be scheduled preemptively, first by using the built-in round robin scheduler with potential improvements later on.

2) What data structures will you need to add?  Be specific as possible here, perhaps even writing out the structs you will be adding to or defining.

We will need to update the `proc` struct to include significantly more information, including a Process ID, a pointer to its thread, information about its opened files, potentially CPU burst data, its return value (possibly -1 for unterminated processes), a sleeplock for use in handling concurrency issues, and links and metadata concerning parent and child processes. We will also need to create a file data struct for use by `proc` structs to keep track of each open file. a `file_data` struct would include the file descriptor, a pointer to the file's vnode, a current seek position, the status (i.e., R/W/A) of the file, and synchronization primitives to ensure I/O operations are threadsafe. Presently, these are the most significant data structures we anticipate implementing. As we move through the project, however, we may find that others not listed here or yet conceived may be needed, so this is by no means exhaustive.

3) How will you handle process termination?  How will this effect parent / child processes?

Our overall strategy for process termination is to implement cascading termination, such that when a process terminates it also terminates any running child processes. Since we do not presently intend to add user process multi-threading, a process can be said to terminate when the thread it is running on terminates. In this situation, the thread must inform the `proc` struct representing the process that it has terminated along with its return value. This information is then stored in the `proc` struct, which remains allocated and pointed to by its parent process until the parent process releases it. Thus, the final deallocation of memory and destruction of a `proc` must be handled by its parent. This means that when a process terminates it must inform its parent that it has terminated as well as terminating its children (because processes are bad parents, apparently). 

4) How does your implementation protect the kernel data structures?

Significant use will be made of locks in order to make all necessary kernel data structure changes atomic. Additionally, kernel data structures will only be accessible (create, modify or destroy) by kernel processes, user processes will ned to use syscalls which trap into kernel mode (e.g., fork(), waitpid(), open(), etc.) in order to use kernel data structures. Kernel data structues, and in particular the `proc` struct, will likely be made into monitors as well.

5) How does your implementation protect the kernel and user processes from rogue processes?

We will use defensive programming to protect the OS against poorly written or misbehaving processes without crashing the OS. We will make sure to check all the preconditions of syscalls (e.g., non-null pointers) before running any code and return the appropriate error number to the user program. Similarly, the syscall code should always check the error code of other OS subsystems and return an error code to the user program. For example, if the user program fork bombs the OS, eventually the OS will run out of memory and the next fork syscall should terminate when allocating memory fails. 
    
6) What kind of scheduling policy will you implement?  Why?

We will begin by using the default scheduler (1 second round robin) and will improve on it later in the project if we have time. At the moment we feel the built in scheduler is sufficient to begin the project since it allows many concurrent kernel threads, some of which will be mapped to user processes. Thus, user processes will be able to run concurrently. In the future we may look at pre-emptive SJF or even priority scheduling depending on available time.

7) How will you manage concurrent file accesses? 

We will not allow concurrent file accesses. Each file will contain a mutex lock which must be acquired before a user program can read or write to it. We considered implementing Read-Write locks but this seems like more work than what is needed. Read-Write locks are an enhancement to allow for concurrent file reads which isn't required.

8) How will you deal with transferring data from user space to kernel space to user space (e.g., when you are writing `exec()`)? What built-in features of OS/161 will you use to do this?

As discussed in class, transferring data from kernel to user space and vice-versa is primarily managed by the `copyout()` and `copyin()` methods. Whichever syscalls and other methods need to move between user- and kernel-space will know precisely what kind of data they are moving from one to the other and the size of the data. In `exec()`, for example, it is likely that program data will be moved, which may be a `proc` or something like a `switchframe` struct which contains registers, program code, and so on. Because we know ahead of time what kind of data and what amount of data is being transferred, `copyin()` and `copyout()` can be used carefully.

# Syscalls
<!---
What which of your components and OS/161 with it interface with?
What kernel data structures will it have to access and modify?
Will synchronization be required to protect data structures or communicate between processes / kernel threads?
What error states can occur and how will you handle them?
Will data need to be moved between user space and kernel space?

For fork, execv, and waitpid you should attempt to be as detailed as possible for how you expect to implemented them.
--->

`open()`: This syscall must interface with the VFS to get a file descriptor and a vnode. It must update the current PCB's
open file list with the newly opened file. This includes initializing metadata about the newly opened file such as read/
write/open flag, and seek location. Since this operation must be atomic on the PCB, it will have to acquire the internal 
PCB lock before writing to the data structure. The string pathname will need to be copied into kernel memory from 
userland. There are a lot of filesystem specific errors that can occur. Most notably, errors occur when the pathname of 
the file is invalid, or there is no more memory. 

`read()`: This syscall must interface with the VFS to read data from disk or from `STDIN`. Similarly, it must check the 
current PCB to see if the current process has opened the specified file descriptor for reading. If not an error is 
returned. An error is also returned if a hardware error occurs while reading the data. The data read must be copied from 
the kernel into userland. This function must update the current seek position stored in the list of open files in the
PCB. Before reading can occur, this function must obtain the lock for the specified `vnode`. 

`write()`: This syscall must interface with the VFS to write data to the disk or to `STDOUT`/`STDERR`. It must check the
current PCB to see if the current process has opened the specified file descriptor ofr writing. If not, an error is 
returned. Errors are also returned if there is no free space left for writing or the pointer to userdata to be written 
is invalid. If the user pointer is valid, then user data must be copied into the kernel before it can be written to the
file system. This function must obtain the lock for the `vnode` before writing to the file and it must update the 
current seek position in the PCB open file list by the number of bytes written.

`lseek()`: This syscall needs to update the open file seek position maintained by the open file list in the current PCB.
This syscall returns errors if the specified file desciptor is invalid or does not support seeking. If the specified seek
position and offset are invalid, an error is returned. This syscall must acquire the internal PCB lock and release it
once finished.

`close()`: This syscall needs to interface with the PCB to remove the file from the open file list in the PCB. Open files
in the PCB will have refcounts so that the last process to close a file will be responsible for closing the file in the
VFS layer. An error is returned if closing the file fails or the specified file descriptor is invalid.

`dup2()`: This syscall needs to interface with the open file list in the PCB to read the input file descriptor. It also
needs to interface with the VFS because it needs to close any open files matching the new file descriptor. Errors occur
if the input file descriptors are invalid or if no new files can be open because of file table limits. No data must be
copied from userland but it must be atomic on the PCB. Special cases are when the syscall is copying an open file to
`STDIN`/`STDOUT`/`STDERR`.

`chdir()`: This sycall updates the current working directory `vnode` inside of the current PCB. Therefore, it must
acquire the internal PCB lock to be atomic. The syscall must copy in the pathname from userland into the kernel. If the
pointer is invalid or the pathname is invalid, then an error is returned.

`__getcwd()`: This syscall looks into the current PCB and returns the name of the current working directory vnode. In 
doing so, it must copy the string out of the kernel and into userland. It will need to be atomic on the PCB so it will
have to acquire the internal PCB lock before reading from the data structure. The syscall returns errors if the userland
buffer pointer is invalid (e.g, `NULL`), reading from disk fails, or the working directory pathname no longer exist. 

`getpid()`: This syscall simply reads the integer PID from the current PCB and returns it. No data must be moved out of
the kernel and no errors can occur. This method does need to be atomic on the PCB data structure. 

`fork()`: This syscall must interact with the current process' PCB in order to copy the process' state, but must also 
interface with the CPU abstraction and the kernel process in order to allocate a new kernel thread for the new process
and add this thread to the CPU's `runqueue`. In order to create a new thread, duplicate program state, and so on, the 
`fork()` syscall must also work with the VM through the `copyout()` and possibly `copyin()` methods. The creation of a
new process via `fork()` should presumably be atomic within the parent process, such that once the `fork()` call is made
by the parent, the parent does not continue to run until the new process is fully created. As such, synchornization will
be needed to ensure `fork()` returns in its program before the code is allowed to continue. Additionally, `fork()` will
likely make otehr syscalls and access other kernel data structures like the VM or the VFS which will need to be 
synchronized. The primary error we anticipate with `fork()` is a lack of memory to allocate for a new process. In essence,
`fork()` must create a new thread and a new PCB, link these together, duplicate the current process' state from its PCB,
load this into the new PCB, then send the new thread to the `runqueue`.

`execv()`: This syscall will need to tightly interact with the VFS and the VM in order to find, read, and copy program
code into a PCB and have it be executed properly. It will need to modify both PCBs and threads, as well as working with
vnodes to load data. Synchronization will be necessary to ensure that the entire program code is loaded into the new PCB
before allowing the new process to run. There may also be concurrency issues which must be resolved if multiple processes
attempt to load the same program concurrently, or if the program is somehow being modified during load. Errors from this
syscall will include memory errors, but also invalid program code. The `execv()` syscall will ultimately be directed to a
section of memory in the VFS with program code, which it will then copy into a user-level PCB to be executed.

`waitpid()`: This syscall needs to read the PCB to determine if one of its children has completed. If a child matching 
the specified PID has terminated, then it should return. If no matching child PID is found, then an error is returned.
Otherwise, the current thread should sleep and only awoken when the corresponding child signals the thread via the 
`exit()` system call. The PCB will contain a semaphore which `waitpid()` waits on and is signaled by the child process 
in `exit()`.   

`_exit()`: This syscall needs to update the PCB's return value using the specified exit code after acquiring the PCB's
internal lock. In addition, the syscall needs to close all open files. Since we are implementing cascading termination, 
all children processes of the current process are also  terminated. If the process has no parent (i.e., it was created 
by the kernel and not forked by a user program), then the syscall can cleanup the PCB and any associated data. 
Otherwise, the syscall needs to signal to the parent process that it has finished. Parent processes must explicitly wait 
for a child via `waitpid()` otherwise the child will be terminated via cascading termination when the parent calls 
`exit()`.

# Implementation Timeline
<!---
Give a time line of implementation focusing on what components need to be implemented before other components.  
(I'm not looking for deadlines, though you can set those for your own benefit.)
--->

Below is a general idea of which tasks need to come first and which should be done later. Although presented in a numerical
ordering, many of these tasks will be completed concurrently. Once task 1) is complete, for example. task 3) can begin without
task 2) having been complete. Similarly, once `_exit()` is implemented, work can begin on various user process related syscalls
in tasks 5) and 6) even while work is being done on tasks 4), and task 7) can be begun once task 4) is done without concern for
the intervening tasks. What we want to capture here is some sense of task dependency: implementing `fork()` and `execv()` depends
on fleshing out the PCB `proc` struct and being able to `_exit()` threads, but not on having an implemented `openfile` struct or
being able to manipulate files. Similarly, `lseek()`, `read()`, `write()` and so on need the `openfile` struct to work and and
need `open()` and `close()` syscalls to be able to be tested effectively.

1) Add required metadata to PCB.

2) Define `struct openfile` and add open file list to PCB.

3) Implement a simple version of `_exit()` via `thread_exit()`. 

4) Implement a simple version of `open()`, `close()`, and `write()` using `kprintf()`. Test using `testbin/palin`
    
5) Implement `getpid()` and `__getcwd()`.

6) Roughly implement `fork()` and `execv()` syscalls and test

7) Roughly implement `read()`, `lseek()`, and other file syscalls and test

8) Roughly implement `waitPID()` and other file syscalls

9) Polish implemented sycalls and tie up loose ends by implementing remaining syscalls

# Group Work
<!---
Briefly discuss how you are going to work together as a group, how you are going to manage access to the repository, 
and what base code you will be starting from or porting.

Consider how you will fairly split up your implementation among your team members.  Your design must describe in detail 
which team member is responsible which component of your implementation.  Team members will be held individually 
responsible for the successful implementation of these components.
--->

Our group is starting from Aidan's codebase. We are using Slack to communicate. Each group member will have their own
branch to work on. We will be using Gitlab merge requests to review each others code before merging into master. In 
addition, we will be using Gitlab to track issues and TODOs. We may also add TODOS in code comments. Since we are all 
using Clion, it makes it easy for us to find TODOs inside of code.

### Division of Work

*Aidan*:
`open()`, `read()`, `write()`, `_exit()` and associated infrastructure

*James*:
`fork()`, `execv()`, `waitpid()`, and associated infrastructure

*Violet*:
`close()`, `dup2()`, `lseek()`, `chdir()`, `__getcwd()`, `getpid()`,  and associated infrastructure
