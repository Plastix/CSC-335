# Project 3 Status
We have code for all parts of Project 3. Much of the code has subtle bugs which need to be ironed out. There are
definitely memory leaks in our implementation (Probably because de-allocation in `exit()` isn't finalized).

Violet has a bunch of code to handle the remaining unimplemented filesystem syscalls. This code hasn't made it 
into `master` yet.

## Working
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

## Not Finished
- `execv()` (2 - James)
    + Code is written and seems like it should be correct. However, the `bigexec` and `badcall:exec` tests both fail, which indicates something is wrong with the code. My approach to implementing `execv` was to take use the `runprogram` method in `syscall.c` as a prototype and try to adjust its code to work as `execv`. That meant dealing with user/kernel memory movement, primarily getting the program's arguments and placing them in the proper spot on the new program's stack. I suspect that my method of placing the arguments in the stack and giving the new program a pointer to them is faulty. Specifically, I'm not sure where to put the argument strings in memory, and sure determine that.
- `exit()` (3 - Aidan)
    + Code is written to handle cascading termination and de-allocation of PCBs/PCB resources. This currently panics 
    with some `kfree` issues.
    + 
- `lseek()` (# - Violet)
- `dup2()` (# - Violet)
- `chdir()` (# - Violet)
- `getcwd()` (# - Violet)
- 

## Individual Questions
### Aidan: TODO

### James:
The subsystem I learned most about was the address space and process memory management subsystem, which includes `struct addrspace` and `struct trapframe` objects. This subsystem was heavily used in the `fork()` and `execv()` methods, but is also important for `waitpid()`. Also important for `fork()` and `waitpid()` was the `struct thread` object and some understanding of how threads are handled by the CPU. I really wish I knew how to adjustthe memory size. it seems like it is currently quite small, which makes memory leaks a larger problem. Although these leaks should be addressed, I'd like confirmation that my methods work as expected without always crashing from lack of memory. `fork()` and `execv()` were both more difficult than I anticipated because I underestimated how hard figuring out memory issues would be. My primary difficulty was in figuring out exactly how OS/161 manages process memory.


## Future work


## Design Document
We definitely didn't put enough thought and design into the open file table. 
Aidan ended up redesigning the entire file table subsystem. This was a lot
more work than 

## Group Work
Our group worked well together. Despite the fact that we had differing schedules, 
we communicated a lot on Slack and worked on our individual git branches without
much conflict. We were able to get in a few good code reviews which caught some
bugs early on (but apparently not enough). 

## Timeline
???

