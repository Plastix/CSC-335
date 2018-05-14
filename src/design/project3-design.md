# Project 3 Design Document

### Design Choices
What are the major components that you will have to add to OS/161 and how will they interact with OS/161?  To this end you may want to discuss your answers to the following questions.

1) Will there be one or many kernel threads?  Preemptive or non-preemptive.

    We will be using many kernel threads which are preemptively scheduled. 

2) What data structures will you need to add?  Be specific as possible here, perhaps even writing out the structs you will be adding to or defining.

    - Update proc struct
    
    - File metadata struct (struct openfile) 
    This struct will contain data such as the file descriptor, seek position, read/write/append flag information, and 
    the underlying vnode. 
    
    TODO

3) How will you handle process termination?  How will this effect parent / child processes?

    We are using cascading termination of processes.
    TODO

4) How does your implementation protect the kernel data structures?

    - Locks will be used to protect against concurrency issues
    - Some data structures will be made as monitors
        - pcb
    TODO

5) How does your implementation protect the kernel and user processes from rogue processes?

    We will use defensive programming to protect the OS against poorly written or misbehaving processes without crashing
    the OS. We will make sure to check all the preconditions of syscalls (e.g., non-null pointers) before running any 
    code and return the appropriate error number to the user program. Similarly, the syscall code should always check
    the error code of other OS subsystems and return an error code to the user program. For example, if the user program
    fork bombs the OS, eventually the OS will run out of memory and the next fork syscall should terminate when allocating
    memory fails. 
    
6) What kind of scheduling policy will you implement?  Why?

    We will begin by using the default scheduler (1 second Round Robin) and will improve on it later in the project if
    we have time.

7) How will you manage concurrent file accesses? 

    We will not allow concurrent file accesses. Each file will contain a mutex lock which must be acquired before a
    user program can read or write to it. We considered implementing Read-Write locks but this seems like more work than
    what is needed. Read-Write locks are an enhancement to allow for concurrent file reads which isn't required.

8) How will you deal with transferring data from user space to kernel space to user space (e.g., when you are writing exec)?  
What built-in features of OS/161 will you use to do this?

    TODO

### Syscalls
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

`close()`: 

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

`fork()`:

`execv()`:

`waitpid()`:

`_exit()`: This syscall needs to update the PCB's return value using the specified exit code after acquiring the PCB's
internal lock. TODO

### Implementation Timeline
<!---
Give a time line of implementation focusing on what components need to be implemented before other components.  
(I'm not looking for deadlines, though you can set those for your own benefit.)
--->


1) Add metadata to PCB.
    - Define `struct openfile` and add open file list to PCB.
    
2) Implement `getpid()` and `__getcwd()`.
 
3) Implement a simple version of `_exit()` via `thread_exit()`.

4) Implement a simple version of `write`using `kprintf()`.
    - Test using `testbin/palin`
    
5) TODO


### Group Work
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

**Division of work:**

*Violet*:
TODO

*James*:
TODO

*Aidan*:
TODO