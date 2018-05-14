# Project 3 Design Document

### Design Choices
What are the major components that you will have to add to OS/161 and how will they interact with OS/161?  To this end you may want to discuss your answers to the following questions.

1) Will there be one or many kernel threads?  Preemptive or non-preemptive.

1) What data structures will you need to add?  Be specific as possible here, perhaps even writing out the structs you will be adding to or defining.

1) How will you handle process termination?  How will this effect parent / child processes?

1) How does your implementation protect the kernel data structures?

1) How does your implementation protect the kernel and user processes from rogue processes?

1) What kind of scheduling policy will you implement?  Why?

1) How will you manage concurrent file accesses? 

1) How will you deal with transferring data from user space to kernel space to user space (e.g., when you are writing exec)?  What built-in features of OS/161 will you use to do this?

### Syscalls
For each system call you should discuss the following (reflect on and refer to the man pages for the syscall while discussing it) :

1) What which of your components and OS/161 with it interface with?
1) What kernel data structures will it have to access and modify?
1) Will synchronization be required to protect data structures or communicate between processes / kernel threads?
1) What error states can occur and how will you handle them?
1) Will data need to be moved between user space and kernel space?
1) For fork, execv, and waitpid you should attempt to be as detailed as possible for how you expect to implemented them.


### Implementation Timeline
Give a time line of implementation focusing on what components need to be implemented before other components.  
(I'm not looking for deadlines, though you can set those for your own benefit.)

### Group Work
Briefly discuss how you are going to work together as a group, how you are going to manage access to the repository, 
and what base code you will be starting from or porting.