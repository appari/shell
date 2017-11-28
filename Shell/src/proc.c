#include "../include/proc.h"

const size_t proc_size = sizeof(proc);

void add(int pid, char *cmd, proc **procs){
    if (pid == -1){
        proc *elem = (proc *) shalloc(proc_size);
        elem -> cmd = cmd;
        elem -> next = *procs;
        *procs = elem;
    }
    else
        strcpy((*procs) -> cmd, cmd);

    (*procs) -> pid = pid;
}

void rem(int pid, proc **procs){
    proc *prv = NULL, *curr = *procs;
    for (; curr; prv = curr, curr = curr -> next){
        if (curr -> pid == pid){
            if (prv)
                prv -> next = curr -> next;
            else
                *procs = curr -> next;
            shfree(curr -> cmd, CMD_BUF_SIZE);
            shfree(curr, proc_size);
            break;
        }
    }
}

void clear(proc *procs){
    proc *prv, *curr;
    for (curr = procs, prv = NULL; curr; prv = curr, curr = curr -> next){
        if (prv){
            shfree(prv -> cmd, CMD_BUF_SIZE);
            shfree(prv, proc_size);
        }
    }

    if (prv){
        shfree(prv -> cmd, CMD_BUF_SIZE);
        shfree(prv, proc_size);
    }
}

proc *get_proc(int job_id, proc *procs){
    static int i = 0;
    if (procs){
        proc *ret = get_proc(job_id, procs -> next);
        if (i++ == job_id && procs -> pid != -1)
            return procs;
        else
            return ret;
    }
    else{
        i = 1;
        return NULL;
    }
}

int proc_len(proc *procs){
    proc *curr;
    int i = 0;
    for (curr = procs; curr; curr = curr -> next)
        if (curr -> pid != -1)
            i++;

    return i;
}

int exists(int pid, proc *procs){
    proc *curr;
    for (curr = procs; curr && curr -> pid != pid; curr = curr -> next);
    return (curr != NULL);
}

char *get_cmd(int pid, proc *procs){
    proc *curr;
    for (curr = procs; curr && curr -> pid != pid; curr = curr -> next);
    return (curr) ? curr -> cmd : NULL;
}

int dead_proc(proc *procs){
    if (procs){
        proc *curr;
        for (curr = procs; curr; curr = curr -> next){
            if (curr -> pid != -1){
                char path[CMD_BUF_SIZE];
                sprintf(path, "/proc/%d", curr -> pid);
                if (access(path, F_OK) == -1)
                    return curr -> pid;
            }
        }
    }

    return -1;
}

void print_procs(proc *procs){
    static int i = 0;
    if (procs){
        print_procs(procs -> next);
        char *state[2]={"Running","Stopped"};
        int x=1;
        if (procs -> pid != -1){
            if(kill(procs->pid,0) == 0)
                x=0;
            printf("[%d] %s %s [%d]\n", i++, state[x], procs -> cmd, procs -> pid);
        }
    }
    else
        i = 1;
}

void *shalloc(size_t size){
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED){
        perror("Shared memory allocation failure");
        exit(1);
    }

    return ptr;
}

void shfree(void *ptr, size_t size){
    munmap(ptr, size);
}
