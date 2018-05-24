# Project 3 Status
We have code for all parts of Project 3. Much of the code has subtle bugs which need to be ironed out. There are
definitely memory leaks in our implementation (Probably because de-allocation in `exit()` isn't finalized).

Violet has a bunch of code to handle the remaining unimplemented filesystem syscalls. This code hasn't made it 
into `master` yet.

## Working
- `open()`
- `read()`
- `write()`
- `close()`
- `getpid()`


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
- `fork()`
- `execv()`
- `waitpid()`
- `exit()`
    - Code is written to handle cascading termination and de-allocation of PCBs/PCB resources. This currently panics 
    with some `kfree` issues.
- `lseek()`
- `dup2()`
- `chdir()`
- `getcwd()`