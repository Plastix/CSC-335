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
We now maintain both a global file table and a per-process local file table. We need a global table in order to share
locks and `vnodes` between different processes. 

We have two monitor structs defined in `filetable.h`:
- `Global_File_Table`:
    - Contains `File` objects. These contain:
        - `struct vnode *vnode` - The open file itself
        - `struct lock *lk` - For making file read/writes atomic
        - `bool is_standard` - Notes that that this file is for STD* and should not be removed.
-  `Local_File_Table`:
    - Contains `File_Desc` objects. These contain
        - `File *file` - Pointer to global open file
        - `int flags` - Flags of the opened file. This is read-only, write-only, or read-write.
        - `off_t seek_location` - Place in the file
        - `struct lock *lk` - For protecting seek location
        - `int standard` - Denotes that this open file is STDIN/STDOUT/STDERR
        
`filetable.h` contains monitor methods for operating on both global and local file tables. These montior methods do much
of the heavy lifting for the filesystem syscalls. Both file tables are implemented with fixed size arrays for ease of
implementation. Local file tables have a max size of 16 files and the global file table has a max size of 64 files.


`file_table_bootstrap()` inits the global file table and is called at the end of system boot in `main`. When the global
file table initializes, it sets three global pointer objects:
- `File_Desc *stdin_fd`
- `File_Desc *stdout_fd`
- `File_Desc *stderr_fd`

This means that when duping STD*, we can just copy the pointer. These global objects are made sure never to be closed.
When a local file table is initialized, the first three indices are set to point to these objects.

## Syscalls
- We set a maximum user specified filename length of 128 characters. 
- `open()` copies the user-space filename into the a kernel buffer allocated on the stack.
- `read()/write()` copy data in/out of user-space using heap buffers since `testbin/palin` can overflow the stack.

