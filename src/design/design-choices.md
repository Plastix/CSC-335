# Internal Design Choice Document

## `strut proc`
The main process abstraction is the `struct proc` data structure. This includes all the data needed for the process to be run. 

*Design Choices*: 

 - There is a max number of children, currently  (the MAX_CHILDS macro in proc.h)
    - `fork()` returns an error if there are already 32 children
 - Children are stored as an array of `struct proc` pointers
 - PIDs are not directly stored, they are accessible through the parent or child pointers 
 - We'll need a global table of `struct proc`s so that `waitpid()` can look up the specified process
 - We add a `struct cv waiting`field to `struct proc` so that processes looking to use `waitpid()` on target process `x` can call `cv_wait()` on the `cv waiting` of `x` and `x` can call `cv_broadcast_all()` on its `waiting` cv when it terminates.