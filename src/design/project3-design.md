# Project 3 Design Document

### Design Choices
```markdown
What are the major components that you will have to add to OS/161 and how will they interact with OS/161?  To this end you may want to discuss your answers to the following questions.

    Will there be one or many kernel threads?  Preemptive or non-preemptive.
    What data structures will you need to add?  Be specific as possible here, perhaps even writing out the structs you will be adding to or defining.
    How will you handle process termination?  How will this effect parent / child processes?
    How does your implementation protect the kernel data structures?
    How does your implementation protect the kernel and user processes from rogue processes?
    What kind of scheduling policy will you implement?  Why?
    How will you manage concurrent file accesses? 
    How will you deal with transferring data from user space to kernel space to user space (e.g., when you are writing exec)?  What built-in features of OS/161 will you use to do this?
```
TODO

### Syscalls
```markdown
For each system call you should discuss the following (reflect on and refer to the man pages for the syscall while discussing it) :

    What which of your components and OS/161 with it interface with?
    What kernel data structures will it have to access and modify?
    Will synchronization be required to protect data structures or communicate between processes / kernel threads?
    What error states can occur and how will you handle them?
    Will data need to be moved between user space and kernel space?
    For fork, execv, and waitpid you should attempt to be as detailed as possible for how you expect to implemented them.
```
TODO


### Implementation Timeline
```markdown
Give a time line of implementation focusing on what components need to be implemented before other components.  
(I'm not looking for deadlines, though you can set those for your own benefit.)
```
TODO

### Group Work
```markdown
Briefly discuss how you are going to work together as a group, how you are going to manage access to the repository, 
and what base code you will be starting from or porting.
```
TODO