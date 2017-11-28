#ifndef SHELL_H
#define SHELL_H

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>
#include <sys/mman.h>
#include "input.h"
#include "proc.h"
#include "command.h"
#include "output.h"

#define PIPE_BUF_SIZE 1024

typedef struct{
    int old_in, old_out;
	int new_in, new_out;
} iofds;

extern const size_t io_size;


void run(char *cmd);
void runcmd(char *cmd);
void runsys(char *cmd);
iofds *redirection(char **cmd);

void int_handler(int signum);
void stop_handler(int signum, siginfo_t *siginfo, void *context);
void chld_handler(int signum, siginfo_t *siginfo, void *context);
char home[HNAME_SIZE];
#endif
