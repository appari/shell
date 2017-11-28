#ifndef PROC_H
#define PROC_H

#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define CMD_BUF_SIZE 1024

struct proc{
    int pid;
    char *cmd;
    struct proc *next;
};

typedef struct proc proc;
extern const size_t proc_size;

void add(int pid, char *cmd, proc **procs);
void rem(int pid, proc **procs);
void clear(proc *procs);
proc *get_proc(int job_id, proc *pids);
int exists(int pid, proc *procs);
int proc_len(proc *procs);
char *get_cmd(int pid, proc *procs);
int dead_proc(proc *procs);
void print_procs(proc *procs);
void *shalloc(size_t size);
void shfree(void *ptr, size_t size);

#endif
