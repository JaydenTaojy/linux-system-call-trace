# linux-system-call-trace-

Setup

Have pin downloaded. pin-3.13-98189-g60a6ef199-gcc-linuxis the version used here, just in case.
Pin directory is in the ~/ directory (/home/sekar)
Place tar-file into in ~/(pinDIR)/source/tools/
When untar-ing, you will get a folder 'CSE509HW3'
Running "make" or "make all" should compile code for all projects
Part 1: Pintools Warmups

Pintool 1: Malloc Tracer/Counter

Counts the number of Malloc(3) calls used in an application, their return values, and at the end of the file, writes the number of bytes alocated.

Usage:

From inside pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3: ../../../pin -t obj-ia32/pinMalloc.so -- <Your Application>

Errata:

Firefox doesn't seem to want to return the number of allocated calls or bytes allocated, even after waiting for a full loading of the application.

Pintool 2: Basic Block Counter

Counts the number of Basic Blocks (code blocks without any jumps or branches to other code pieces) executed in a program. .

Usage

From inside pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3: ../../../pin -t obj-ia32/pinBlock.so -- <Your Application>

Errata:

Firefox still doesn't want to return anything, unlike other arbitrary programs like ls, vim, etc.

Pintool 3: Direct/Indirect Control Transfer/Return/Branch Count

Counts all instances of direct/indirect control flow. This includes branches, returns, function calls, and syscalls.

Usage

From inside pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3: ../../../pin -t obj-ia32/pinCtrl.so -- <Your Application>

ErrataL

Part 2: Security Application: "bTraceApp", or p(in)Trace

Performs system call interception with Pin calls inside Basic Blocks
Usage:

From inside pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3: ../../../pin -t obj-ia32/btraceApp.so -- <Your Application>

Breakdown

Breaks down basic blocks in a trace, where if a given instruction is a system call, every argument w.r.t said call is printed, along with an associated name. The start of the next basic block is after the return from kernel space, which allows the return value from EAX to be processed. Error numbers are provided if the returned errno variable is NOT zero.




The way to build the project: 
1. Copy the homework directory to .../pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/' 
Where  "pin-3.13-98189-g60a6ef199-gcc-linux" is the main directory of pin I used. 

2. Go to the homework directory and then run make to get all pin tools for this assignment: 1.1 bblcount.cpp and bblcount_mt.cpp (multi thread version) to bblcount.so and bblcount_mt.so; 1.2 malloctrace.cpp to malloctrace.cpp (support multi thread); 1.3 direct.cpp to direct.so (not support multi thread). 
Problem 2 I choose to build btrace and I provide two version: breace.cpp to btrace.so (no support multi thread) and breace_mt.cpp to btrace_mt.so (support multi thread). 

3. After build run each part as the pin tool command： ../../../pin -t obj-ia32/....so -- command to test the result. Each tool will write a .log file to record the running result:  bblcount.log and bblcount_mt.log for 1.1; malloctrace.log for 1.2; directcount.log for 1.3; btrace.log and btrace_mt.log for btrace implementation. 

The format of btrace.log is: syscallname(arguments(not all of them are parsed))
                            Return: return value.
The format of btrace_mt.log is: thread id: syscallname(arguments)
                                thread id: Return: return value.
For btrace_mt.so, I have tested the command as gedit table.txt for multi thread test.

For others except malloctrace.so, I use command as /bin/ls and /bin/ls -l for test.

4. After running, run make clean to remove the .so directory.

5. The table.txt is used for btrace to setup the syscall number and name map: I just get sys call table from https://syscalls.kernelgrok.com and change sys_exit to exit as the name of syscall, so some syscall names may be not accurate 100%.
