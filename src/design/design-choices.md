# Internal Design Choice Document

## `strut proc` 
 - There is a max number of children, currently  (the `MAX_CHILDS` macro in `proc.h`)
    - `fork()` returns an error if there are already 32 children
 - Children are stored as an array of `struct proc` pointers. Since we need to keep the PCB around after `exit()`, 
 `waitpid()` is responsible for nulling out the slot in the array.
 - PIDs are not directly stored, they are accessible through the parent or child pointers 
 - We have a global fixed size array of `struct proc`s so that `waitpid()` can look up the specified process by PID 
 index.
 - PCBs contain a `struct cv waiting`field to `struct proc` so that processes looking to use `waitpid()` on target process 
 `x` can call `cv_wait()` on the `cv waiting` of `x` and `x` can call `cv_broadcast_all()` on its `waiting` cv when it 
 terminates.
 - A process can look up the state of a child process by inspecting the state of the child's thread.
 
 
## File System
We now maintain both a global file table and a per-process local file table. Each of these objects is a monitor.



