#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>


#define HNAME_SIZE 1024
#define PATH_SIZE 1024

void prompt(char *);
char *relative(char *abs,char *);


#endif
