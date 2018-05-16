# Internal Design Choice Document

## `strut proc`
The main process abstraction is the `struct proc` data structure. This includes all the data needed for the process to be run. 

*Design Choices*: 

 - There is a max number of children, currently  (the MAX_CHILDS macro in proc.h)
    - `fork()` returns an error if there are already 32 children
 - Children are stored as an array of `struct proc` pointers
 - PIDs are not directly stored, they are accessible through the parent or child pointers 
 