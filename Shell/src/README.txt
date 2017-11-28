input.c

This file contains code to read in the input from the shell. It uses fgets
to read input and stores it in a buffer of 512 bytes. The code makes sure
that all the input is read even when the buffer is full by looping through
the input until the end. It returns one command at a time out of the many
semi-colon separated commands by using static variables in the function 
much like standard C I/O functions. This module also contains code to 
return an array of tokens by specifying a delimiter given a string as well
as a function to append a string to a given array of strings.

--------
output.c

This file mainly contains code to prompt the user. It uses a helper 
function to determine the current working directory and prints the 
prompt accordingly.

---------
command.c

This file contains the implementation of all the built-in commands 
comprising cd, pwd, echo and pinfo. Unlike the rest of the commands in
this module, pinfo doesn't print directly. Instead it returns an array 
of strings so that it can be used to print the name of the process 
when a background process exits.

-------
shell.c

The program reads one semi-colon separated command at a time and 
decides which function call to make depending on the input. If the 
command to be executed is a system command it forks the current process
and uses execvp in the child process to run the command. It also uses 
tokenize method from input.c module to split the command string by '&' 
(when multiple commands are run in the background at once) and by ' ' to pass 
the arguments to execvp. 

The parent process doesn't wait for the child process when the process
is to be run in the background. So, to print the process name and its pid
when a process exits a signal handler is used for SIGCHLD signal which 
is sent when a child process is terminated. We also have to wait on this
process (without blocking) to prevent the child process from becoming a
zombie process. But, in order to get the process name we have to 
access the process entry table stored in proc virtual file system before
calling wait as it clears the entry for the exited process.

--------------------
Execution Directions

Type "make" to compile all the modules and create the executable.
Type "make run" or "./shell" to run the shell.
Type "make clean" to remove object files and executable from source directory.
