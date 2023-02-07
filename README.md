# PMan
CSC360 Assignment 1

## Commands
This assignment uses system calls to execute basic shell commands.
1. bg (executable) (optional parameters) - to run executable in the background
2. bglist - to list background processes
3. bgkill (pid) - to kill the specified process
4. bgstop (pid) - to stop the execution of the specified process
5. bgstart (pid) - to resume the execution of the specifed process

### bg (executable)(optional parameters)
* creates a child process to execute the given program
* adds the pid of the child process to activeProcesses Linkedlist
* prints error message if executable was not found 

* prints message after process has terminated (before the next shell prompt is printed)

### bglist
* prints the contents of the activeProcesses Linkedlist
* prints message if activeProcesses is empty

### bgkill (pid)
* sends TERM signal to the specified process pid
* prints confirmation message if successful
* prints error message if pid is invalid

### bgstop (pid)
* sends STOP signal to the specified process pid
* prints confirmation message if successful
* prints error message if pid is invalid

### bgstart (pid)
* sends CONT signal to the specified process pid
* prints confirmation message if successful
* prints error message if pid is invalid

### exit
* exit command will exit PMan.c program


