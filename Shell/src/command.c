#include "../include/command.h"
#include <errno.h>

void cd(char *path){
	if (!path || strcmp(path, "~") == 0){
		if (chdir(getpwuid(getuid()) -> pw_dir) == -1)
			perror("");
	}
	else if (chdir(path) == -1)
		perror("");
}

void pwd(){
	char cdir[PATH_SIZE];
	getcwd(cdir, PATH_SIZE);
	puts(cdir);
}

void echo(char **strs, int num_strs){
	while (num_strs-- > 1)
		printf("%s ", *strs++);

	if (strs)
		puts(*strs);
	else
		puts("");
}

void pinfo(char **tokens, int num_tok){
    pid_t pid;
    if (num_tok > 1)
        pid = (pid_t) strtol(tokens[1], (char **) NULL, 10);
    else
        pid = getpid();

    char *attrs[3] = {"Pid", "State", "VmSize"};
    char **vals = pinfo_helper(pid, attrs, 3);
    if (vals){
        char *format = "%s -- %s\n";
        int i;

        printf(format, "Pid", vals[0]);
        printf(format, "Process Status", vals[1]);
        printf(format, "Memory", vals[2]);

        for (i = 0; i < 3; i++)
            free(vals[i]);

        char path[PATH_SIZE], buf[PBUF_SIZE];
        memset(path, 0, PATH_SIZE);
        memset(buf, 0, PBUF_SIZE);
        sprintf(path, "/proc/%d/exe", pid);
        readlink(path, buf, PBUF_SIZE);
        printf(format, "Executable Path", buf);
    }
    else
        fprintf(stderr, "No such process exists\n");
}

char **pinfo_helper(pid_t pid, char **attrs, int attr_len){
	char path[PATH_SIZE], buf[PBUF_SIZE],
		 attr[PBUF_SIZE], val[PBUF_SIZE],
		 done[attr_len];
	char *ret;
	int i;
    for (i = 0; i < attr_len; i++)
        done[i] = 0;

	sprintf(path, "/proc/%d/status", pid);
	FILE *file = fopen(path, "r");

	if (file){
		while (fgets(buf, PBUF_SIZE, file)){
			sscanf(buf, "%[^:]: %s", attr, val);
			for (i = 0; i < attr_len; i++){
				if (!done[i] && strcmp(attrs[i], attr) == 0){
					ret = (char *) malloc(strlen(val) + 1);
					strcpy(ret, val);
					attrs[i] = ret;
					done[i] = 1;
				}
			}
		}

		fclose(file);
		return attrs;
	}
	else
		return NULL;
}

void jobs(proc *procs){
    if (procs -> next){
        print_procs(procs);
    }
    else
        puts("No jobs currently running");
}

void kjob(int job_id, int sig_id, proc *procs){
    proc *job = get_proc(job_id, procs);
    if (job)
        kill(job -> pid, sig_id);
    else
        fprintf(stderr, "No such job exists!\n");
}

void fg(int job_id, proc *procs){
    proc *job = get_proc(job_id, procs);
    if (job){
        pid_t pid = job -> pid, pgid = getpgid(pid),
                prv_pgid = getpgid(getpid()), ret, ppid;
        char *attrs[1] = {"PPid"};
        char **vals = pinfo_helper(pid, attrs, 1);
        ppid = (pid_t) atoi(*vals);
        printf("pid: %d, ppid: %d\n", pid, ppid);

        tcsetpgrp(STDIN_FILENO, pgid);
        do{
            ret = waitpid(ppid, NULL, 0);
        } while (ret == -1 && errno == EINTR);
        if (ret == -1)
            perror("Wait Error");
        tcsetpgrp(STDIN_FILENO, prv_pgid);
    }
}

void overkill(proc *procs){
    if (procs){
        proc *curr;
        for (curr = procs -> next; curr; curr = curr -> next)
            kill(curr -> pid, SIGKILL);
    }
}

void quit(proc *procs){
    clear(procs);
    killpg(getgid(), SIGKILL);
    exit(0);
}

int check_prefix(char *str1, char *str2){
	int i;
	while (*str1 == ' ')
		str1++;
	for (i = 0; str1[i] && str2[i] && str1[i] == str2[i]; i++);
	return !str2[i];
}

int open_file(char *name, int read, int over_write){
	int read_flags = O_RDONLY, ret;
	int write_flags = O_WRONLY | O_CREAT | (over_write ? O_TRUNC : O_APPEND);
    if (read)
        ret = open(name, read_flags);
    else
        ret = open(name, write_flags, 0644);

	if (!ret)
        perror("Failed to open file");
	return (ret) ? ret : ((read) ? 0 : 1);
}
