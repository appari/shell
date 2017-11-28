#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include "proc.h"

#define PATH_SIZE 1024
#define PBUF_SIZE 1024


void cd(char *path);
void pwd();
void echo(char **strs, int num_strs);
void pinfo(char **tokens, int num_tok);
void jobs(proc *pids);
void kjob(int job_id, int sig_id, proc *pids);
void fg(int job_id, proc *pids);
void overkill(proc *pids);
void quit(proc *pids);

char **pinfo_helper(pid_t pid, char **attrs, int attr_len);
int check_prefix(char *str1, char *str2);
int open_file(char *name, int read, int over_write);

#endif
