# os161-gdp
James, Violet, and Aidan's operating system.

### Installing & Running
1. `./configure PROJ3`
2. `./boot.sh`

### Custom Scripts
We have a few custom scripts for improving the build workflow:
* `./configure.sh <CONF>`: Configures the OS and compiles both userland and the kernel given a config file.
* `./compile.sh`: Compiles the kernel code.
* `./boot.sh`: Boots the OS.
* `./run.sh`: Compiles the kernel code and if successful, boots the OS.
* `./debug.sh`: Launches the os161 specific gdb debugger.

`boot.sh` and `run.sh` accept optional sys161 params. For example `./boot.sh -w` will boot up the OS and wait for the 
debugger to attach.

### GDB Settings
GDB is setup to automatically connect to the OS via the `root/.gdbinit` file. If you want to setup breakpoints 
automatically or define custom macros edit that file.

### Implementation Information
- For design changes from the original design document see the [design changes document](src/design/design-choices.md)
- See the [status document](src/design/project3-status.md) for information about what is implemented and working.