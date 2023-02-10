# PMan
CSC360 Assignment 1

## Commands
This assignment uses system calls to execute basic shell commands.
1. bg (executable) (optional parameters) - to run executable in the background
2. bglist - to list background processes
3. bgkill (pid) - to kill the specified process
4. bgstop (pid) - to stop the execution of the specified process
5. bgstart (pid) - to resume the execution of the specifed process

### make
* create the executable 

### make run
* to update executable (if changed)
* to run ./PMan

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
* exits PMan.c program

## Functions

### main
* initializes the Linkedlist activeProcesses
* initializes char* input to store the readline
* initializes char* parsedCmd[] to store the tokenized version of input
* while(1) loop to get and parse user input 
* free() any dynamic memory used

### executeCmd
* determine which command to execute
* forks for a child process if used with the bg command
* handles update or error messages 

### createArgs
* creates a char* args[] to pass into execvp() call

### parseInput
* tokenizes the input string into a char* array

### updateActiveProcess
* traverse the activeProcesses Linkedlist to remove terminated processes
* prints update message before removing the process from the list

### printActiveProcesses
* traverses the activeProcesses Linkedlist and prints contents to standard out

### removeProcess
* removes the specified Process from the activeProcesses Linkedlist

### addProcessFront
* adds the pid and name of the forked child to the activeProcesses Linkedlist

