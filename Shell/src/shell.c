#include "../include/shell.h"

const size_t io_size = sizeof(iofds);
const size_t int_size = sizeof(int);
static char buffer[PIPE_BUF_SIZE];
static int *should_wait = NULL;
static proc **procs = NULL;
static int *bg_killed = NULL;

int main(){
    struct sigaction act, act2;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = chld_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &act, NULL);

	memset(&act2, '\0', sizeof(act2));
	act2.sa_sigaction = stop_handler;
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGTSTP, &act2, NULL);
	signal(SIGINT, int_handler);

    should_wait = (int *) shalloc(int_size);
	procs = (proc **) shalloc(sizeof(proc *));
    *procs = NULL;
    bg_killed = (int *) shalloc(int_size);
    *bg_killed = 0;
    getcwd(home,1024);
    cd(home);
    
	char *cmd = NULL;
	while (1){
		prompt(home);

		while (1){
			cmd = input();
			if (cmd){
                if (strcmp(cmd, "\0") == 0 && !*bg_killed){
                    puts("");
                    break;
					//quit(*procs);
				}
                run(cmd);
			}
			else
				break;
		}
	}

	return 0;
}

void run(char *cmd){
	int fds[2];
	int num_tok = 0, tmp = 0, i;
	pid_t pid, wpid;
	char **cmds = tokenize(cmd, "|", &num_tok, &tmp);
	int old_in = dup(STDIN_FILENO);

	for (i = 0; i < num_tok; ++i){
		if (check_prefix(cmd, "quit"))
			quit(*procs);
		if (!(*procs) || (*procs) -> pid != -1){
			char *cmd_cpy = (char *) shalloc(CMD_BUF_SIZE);
			add(-1, cmd_cpy, procs);
		}
		if (pipe(fds) != 0)
			perror("Pipe Failure");

		pid = fork();
		if (pid < 0)
			perror("Fork Failure");
		else if (pid == 0){
			close(fds[0]);
			if (dup2(fds[1], STDOUT_FILENO) == -1){
				perror("Dup2 Failure");
				exit(1);
			}
			iofds *io = redirection(cmds + i);
			runcmd(cmds[i]);

			close(io->new_in);
			close(io->new_out);
			close(io->old_in);
			close(io->old_out);
			free(io);
			exit(0);
		}
		else{
			close(fds[1]);
			if (dup2(fds[0], STDIN_FILENO) == -1){
				perror("Dup2 Failure");
				exit(1);
			}
			pid_t ret;
			do{
				ret = waitpid(pid, NULL, 0);
			} while (ret == -1 && errno == EINTR);
			if (ret == -1)
				perror("Wait Error");

			if (i < num_tok - 1)
				dup2(fds[1], fds[0]);
			else{
				wpid = fork();
				if (wpid < 0)
					perror("Fork Failure");
				else if (wpid == 0){
					while (read(STDIN_FILENO, buffer, PIPE_BUF_SIZE))
						write(STDOUT_FILENO, buffer, PIPE_BUF_SIZE);
					memset(buffer, 0, PIPE_BUF_SIZE);

					close(old_in);
					*bg_killed = 1;
					exit(0);
				}
				else{
                    if (*should_wait){
						do{
							ret = waitpid(wpid, NULL, 0);
						} while (ret == -1 && errno == EINTR);
					}
					int bg_pid = dead_proc(*procs);
					if (*bg_killed && bg_pid != -1){
						printf("\n%s with pid %d exited normally\n",
							   get_cmd(bg_pid, *procs), bg_pid);
						rem(bg_pid, procs);
						*bg_killed = 0;
					}
					dup2(old_in, STDIN_FILENO);
					close(old_in);
				}
			}
		}
	}
}

void runcmd(char *cmd){
    *should_wait = 1;
	int num_tok = 0, tmp = 0;

	if (check_prefix(cmd, "pwd"))
		pwd();
    else if (check_prefix(cmd, "check")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		int pid = (int) strtol(tokens[1], (char **) NULL, 10);
		free(tokens);
	}

	else if (check_prefix(cmd, "cd")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		cd((num_tok > 1) ? tokens[1] : NULL);
		free(tokens);
	}
	else if (check_prefix(cmd, "echo")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		echo((num_tok > 1) ? tokens + 1 : NULL, num_tok - 1);
		free(tokens);
	}
	else if (check_prefix(cmd, "setenv")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		if (num_tok==1 || num_tok >3){
			fputs("Incorrect usage: setenv var [value]\n",stderr);
		}
		else{
			if(setenv(tokens[1],(num_tok==2)?"":tokens[2],1)==0)
				printf("Environment variable set successfully\n");
			else	
				printf("Error\n");
		}

	}
	else if (check_prefix(cmd, "unsetenv")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		if(num_tok == 2){
			if(unsetenv(tokens[1])==0)
				printf("Environment variable unset successfully\n");
			else
				fputs("Error\n",stderr);

		} 
		else
			fputs("Incorrect usage: unsetenv var\n",stderr);
	}
	else if (check_prefix(cmd, "pinfo")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		pinfo(tokens, num_tok);
		free(tokens);
	}
	else if (check_prefix(cmd, "jobs"))
		jobs(*procs);
	else if (check_prefix(cmd, "kjob")){
        char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
        if (num_tok == 3){
			int job_id = (int) strtol(tokens[1], (char **) NULL, 10);
			int sig_id = (int) strtol(tokens[2], (char **) NULL, 10);
			kjob(job_id, sig_id, *procs);
		}
		else
			fputs("Incorrect Usage: kjob <jobid> <sigid>\n", stderr);
        free(tokens);
	}
	else if (check_prefix(cmd, "fg")){
		char **tokens = tokenize(cmd, " ", &num_tok, &tmp);
		int job_id = (int) strtol(tokens[1], (char **) NULL, 10);
        fg(job_id, *procs);
		free(tokens);
	}
	else if (check_prefix(cmd, "overkill"))
		overkill(*procs);

	else
		runsys(cmd);
}

void runsys(char *cmd){
	int num_cmd = 0, len_cmd = 0, size_cmd = 0, bg = 0, i;
	size_t cmd_len = strlen(cmd);
	char **cmds = tokenize(cmd, "&", &num_cmd, &size_cmd), **tokens;
	pid_t pid;

	if (cmd_len != strlen(*cmds))
		bg = 1;

	for (i = 0; i < num_cmd; i++){
        *should_wait = 1;
		len_cmd = 0;
		size_cmd = 0;
		tokens = tokenize(cmds[i], " ", &len_cmd, &size_cmd);
		append(NULL, len_cmd, &tokens, &size_cmd);

		pid = fork();
		if (pid < 0){
			perror("Fork Failure\n");
			exit(1);
		}
		else if (pid == 0){
			execvp(*tokens, tokens);
			fprintf(stderr, "%s: No such command\n", *cmds);
			exit(1);
		}
		else{
			if (bg){
                add(pid, cmds[i], procs);
				printf("[%d] %d\n", proc_len(*procs), pid);
                *should_wait = 0;
			}
            else{
				pid_t ret;
				do{
					ret = waitpid(pid, NULL, 0);
				} while (ret == -1 && errno == EINTR);
				if (ret == -1)
					perror("Wait Error");
			}
		}

		free(tokens);
	}
	free(cmds);
}

iofds *redirection(char **cmd){
	int num_tok = 0, num_rtok = 0, tmp = 0, op_index;
	char **tokens, **rtokens;
	iofds *io = (iofds *) malloc(io_size);
	io -> old_in = dup(STDIN_FILENO);
	io -> old_out = dup(STDOUT_FILENO);

	char **in_tokens = tokenize(*cmd, "<", &num_tok, &tmp);
	if (num_tok > 1){
		rtokens = tokenize(in_tokens[1], " ", &num_rtok, &tmp);
		io -> new_in = open_file(*rtokens, 1, 1);
		dup2(io -> new_in, STDIN_FILENO);

		*cmd = in_tokens[0];
	}

	//Left Side
	num_tok = 0;
	char **out_tokens = tokenize(*cmd, ">", &num_tok, &tmp);
	if (num_tok > 1){
		num_tok = 0;
		tokens = tokenize(out_tokens[1], " ", &num_tok, &tmp);
		op_index = ((int) (out_tokens[1] - out_tokens[0])) - 1;

		if ((*cmd)[op_index] == '>')
			io -> new_out = open_file(*tokens, 0, 0);
		else
			io -> new_out = open_file(*tokens, 0, 1);
		dup2(io -> new_out, STDOUT_FILENO);
		*cmd = out_tokens[0];
		free(tokens);
	}

	//Right Side
	if (num_rtok){
		int i;
		for (i = 0; i < num_rtok; ++i) {
			if (strcmp(rtokens[i], ">>") == 0){
				io -> new_out = open_file(rtokens[i + 1], 0, 0);
				break;
			}
			else if (strcmp(rtokens[i], ">") == 0){
				io -> new_out = open_file(rtokens[i + 1], 0, 1);
				break;
			}
		}

		if (i < num_rtok)
			dup2(io -> new_out, 1);
		free(rtokens);
	}

	free(in_tokens);
	free(out_tokens);
	return io;
}


void int_handler(int signum){
	puts("");
	prompt(home);
    fflush(stdout);
}

void stop_handler(int signum, siginfo_t *siginfo, void *context){
	pid_t pid = siginfo -> si_pid;
	signal(SIGTSTP, SIG_DFL);
	kill(pid, SIGTSTP);

/*	struct sigaction act2;
	memset(&act2, '\0', sizeof(act2));
	act2.sa_sigaction = stop_handler;
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGTSTP, &act2, NULL);*/
	//puts("");

/*	char *attrs[1] = {"Name"};
	char **cmds = pinfo_helper(pid, attrs, 1);
	add(pid, cmds[0], procs);
	printf("Stopped: [%d] %d\n", proc_len(*procs), pid);
	*should_wait = 0;*/
}

void stop_handler2(int signum){
}

void chld_handler(int signum, siginfo_t *siginfo, void *context){
	pid_t pid = siginfo -> si_pid;
	int status;

	if (waitpid(pid, &status, WNOHANG) > 0 && WIFEXITED(status)){
		if (exists(pid, *procs)){
			printf("\n%s with pid %d exited normally1\n",
				   get_cmd(pid, *procs), pid);
			rem(pid, procs);
            *bg_killed = 0;
		}
		else{
			int bg_pid = dead_proc(*procs);
			if (*bg_killed && bg_pid != -1){

				printf("\n%s with pid %d exited normally2\n",
					   get_cmd(bg_pid, *procs), bg_pid);
				rem(bg_pid, procs);
				//*bg_killed = 0;
			}
		}
	}
}

