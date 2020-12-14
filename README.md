# linux-system-call-trace-

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
