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

## Not Finished
- `fork()` (# - James)
- `execv()` (# - James)
- `waitpid()` (# -James)
- `exit()` (3 - Aidan)
    - Code is written to handle cascading termination and de-allocation of PCBs/PCB resources. This currently panics 
    with some `kfree` issues.
    - 
- `lseek()` (# - Violet)
- `dup2()` (# - Violet)
- `chdir()` (# - Violet)
- `getcwd()` (# - Violet)
- 

## Individual Questions
*Aidan*: TODO


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

