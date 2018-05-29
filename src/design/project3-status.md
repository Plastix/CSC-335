# Project 3 Status
We have code for all parts of Project 3. Much of the code has subtle bugs which need to be ironed out. There are
definitely memory leaks in our implementation (Probably because de-allocation in `exit()` isn't finalized).

Violet has a bunch of code to handle the remaining unimplemented filesystem syscalls. This code hasn't made it 
into `master` yet.

## Syscalls: Working
- `open()` (5 - Aidan)
- `read()` (5 - Aidan)
- `write()` (5 - Aidan)
- `close()` (5 - Violet)
- `getpid()` (5 - James)
- `fork()` (5 - James)
- `waitpid()` (5 -James)

These system calls pass the following `testbins`
- `opentest`
- `closetest`
- `readwritetest`
- `consoletest`
- `palin`
- `tictac`
- `badcall:open`
- `badcall:write`
- `badcall:read`
- `badcall:close`
- `testfork`
- `badcall:chdir`
- `badcall:getcwd`

## Syscalls: Not Finished
- `execv()` (2 - James)
    + Code is written and seems like it should be correct. However, the `bigexec` and `badcall:exec` tests both fail, which indicates something is wrong with the code. My approach to implementing `execv` was to take use the `runprogram` method in `syscall.c` as a prototype and try to adjust its code to work as `execv`. That meant dealing with user/kernel memory movement, primarily getting the program's arguments and placing them in the proper spot on the new program's stack. I suspect that my method of placing the arguments in the stack and giving the new program a pointer to them is faulty. Specifically, I'm not sure where to put the argument strings in memory, and sure determine that.
- `exit()` (3 - Aidan)
    + Code is written to handle cascading termination and de-allocation of PCBs/PCB resources. This currently panics 
    with some `kfree` issues. Why would I be getting panics from `kfree` complaining about page tables? Is this what happens when you double free?
    + Killing process threads is not currently handled yet. We will need to move child threads to the zombie list and then exorcise them.
    + I was preoccupied with the filesystem syscalls and left this to the last minute. I wasn't expecting it to be so challenging and this is why the implementation is unfinished. This syscall is split into two parts: `proc_exit()` and `proc_destroy()`. `proc_exit()` is responsible for deallocating all the unnecessary resources inside of the PCB and all the children PCBs. `proc_destroy()` is what actually deallocates the PCB struct itself after `proc_exit()` has been called. Since we need the PCB to be kept around for `waitpid()`, `waitpid()` will make sure to cleanup using `proc_destroy()` once the child is collected.
    + We already have a global lock for protecting the running processes buffer. We will use this lock to make `exit()` atomic.
- `lseek()` (3 - Violet)
    + Code is written and passed majority of the badcall test (panic due to waitpid). Fixed the 64 bits issue but still failing bigseek test.
- `dup2()` (3 - Violet)
    + Code is written and looks like to function mostly correct. It passed all badcall tests except the last one 'copying stdin to test'. Right now I have some dummy codes to prevent copy stdin from panicking, but that need to be fixed later.
    Also I haven't been able to fully test the function of the syscall. When I tried to test the syscall using tests in the testbin, I couldn't find a test that can get to the point that dup2 is called. ('psort' fails on waitpid)
- `chdir()` (3 - Violet)
    + Code is written and passed all badcall tests. Also having trouble to test the function of the syscall. In both rmdirtest and rmtest, chdir is called after mkdir, but mkdir is currently not implemented. 
    + This syscall needs to be tested before pushing to master, but shouldn't have many issues.
- `getcwd()` (3 - Violet)
    + Code is written and passed all badcall tests.   


## Individual Questions
### Aidan:
I learned a lot about the VFS layer in OS161 since I wrote `open()`, `read()`, 
`write()`, and implemented almost all of the file table operations. A few of
my file table operations assume that when you open an already open `vnode` via
`vfs_open()`, you get back the exact same reference and I want to know if this is
true. I encountered quite a few issues because I made very silly programming 
mistakes. I forgot to set the value of a return pointer in one of my file table
methods which was very hard to track down because it caused a `KASSERT` to fail
deep inside of OS161 code.

### James:
The subsystem I learned most about was the address space and process memory 
management subsystem, which includes `struct addrspace` and `struct trapframe` 
objects. This subsystem was heavily used in the `fork()` and `execv()` methods, 
but is also important for `waitpid()`. Also important for `fork()` and `waitpid()` 
was the `struct thread` object and some understanding of how threads are handled 
by the CPU. I really wish I knew how to adjust the memory size. it seems like it 
is currently quite small, which makes memory leaks a larger problem. Although 
these leaks should be addressed, I'd like confirmation that my methods work as 
expected without always crashing from lack of memory. `fork()` and `execv()` 
were both more difficult than I anticipated because I underestimated how hard 
figuring out memory issues would be. My primary difficulty was in figuring out 
exactly how OS/161 manages process memory.


### Violet:
The part I mostly focused on was filesystem calls. I look at what kind of file 
operations can VFS layer handle and how to interface the VFS functions with our 
implementation. I have some trouble testing the syscalls since the 

## Design Document
We definitely didn't put enough thought and design into the open file table. 
Aidan ended up redesigning the entire file table subsystem the Monday before the
project was due. This was a lot more work than anticipated and it meant that our 
design document changed substantally. In addition, we should have defined interfaces
in header files well in advance which would allow group members to write their
code without having to wait on the file table implementation. On the other hand,
defining our internal syscall methods and wiring them up to the syscall dispatcher
was easier than expected. After returning from a syscall, there is already code
that converts errors to `-1` return values and sets the `erno` value for us.


## Group Work
Our group worked well together. Despite the fact that we had differing schedules, 
we communicated a lot on Slack and worked on our individual git branches without
much conflict. We were able to get in a few good code reviews which caught some
bugs early on (but apparently not enough). 

## Future Work & Timeline
We will try to do more peer programming going forward. It is always good to have 
another set of eyes on very complicated code. In addition, we also need to write 
better commit messages so that teammates know what has changed.

1. Meet as as a group and discuss code
2. Review & Merge Violet's syscall code into master 
3. Debug all file system syscalls
3. Debug `execv()`
4. Debug and finish `exit()`
5. Final testing
