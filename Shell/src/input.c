#include "../include/input.h"

const size_t ptr_size = sizeof(char *);
const size_t ch_size = sizeof(char);

static int is_partial(char *str, int len);
static void rmLine(char *str);
static char *concat(char *str1, char *str2);
static void clear(char *str, int len);

char *input(){
	static char str[BUF_SIZE];
	static char **cmds = NULL, *partial_str = NULL;
	static int num_cmd = 0, cmd_i = 0, exhausted = 0, clear_partial;
	int tmp = 0;
	char *cmd = NULL;

	if (exhausted){
		clear(str, BUF_SIZE);
		if (cmds)
			free(cmds);
		cmds = NULL;
		if (partial_str)
			free(partial_str);
		partial_str = NULL;

		num_cmd = 0;
		cmd_i = 0;
		exhausted = 0;
		return NULL;
	}

	if (clear_partial){
		free(partial_str);
		partial_str = NULL;
		clear_partial = 0;
	}

	if (num_cmd){
		if ((--num_cmd) == 0){
			if (is_partial(str, BUF_SIZE)){
				if (partial_str)
					partial_str = (char *) realloc(partial_str,
								  strlen(cmds[cmd_i]) * ch_size);
				else
					partial_str = (char *) malloc(strlen(cmds[cmd_i]) *
												  ch_size);
				strcpy(partial_str, cmds[cmd_i]);
				free(cmds);
			}
			else{
				cmd = cmds[cmd_i++];
				exhausted = 1;
				rmLine(cmd);
				return cmd;
			}
		}
		else
			return cmds[cmd_i++];
	}

	while (fgets(str, BUF_SIZE, stdin)){
		num_cmd = 0;
		cmd_i = 0;
		cmds = tokenize(str, ";", &num_cmd, &tmp);
		if (num_cmd){
			if (partial_str){
				cmds[0] = concat(partial_str, cmds[0]);
				free(partial_str);
				partial_str = NULL;
			}

			if (is_partial(str, BUF_SIZE)){
				partial_str = (char *) malloc((strlen(cmds[cmd_i]) + 1)
											   * ch_size);
				strcpy(partial_str, cmds[cmd_i]);
				clear(str, BUF_SIZE);
				free(cmds);
				continue;
			}
			else{
				cmd = cmds[cmd_i++];
				num_cmd--;
			}

			if (!num_cmd)
				exhausted = 1;

			rmLine(cmd);
			return cmd;
		}
		else{
			if (partial_str){
				free(cmds);
				clear_partial = 1;
				return partial_str;
			}
		}
	}

    if (!num_cmd && !exhausted && !clear_partial)
		return "\0";
	return partial_str;
}

static int is_partial(char *str, int len){
	int i;
	while ((i = --len)){
		if (str[i] == '\n')
			return 0;
		if (str[i])
			return 1;
	}

	return str[0] != '\n';
}

char **tokenize(char *str, const char *delim, int *num_tok, int *size){
	*size = 1;
	char **tokens = (char **) malloc(*size * ptr_size);

	for (char *token = strtok(str, delim); token;
			token = strtok(NULL, delim)){
		if (strcmp(token, "\n") != 0 || !(*num_tok))
			append(token, (*num_tok)++, &tokens, size);
	}

	return tokens;
}

void append(char *str, int len, char ***arr, int *size){
	if (len >= *size){
		*size *= 2;
		*arr = (char **) realloc(*arr, (*size) * ptr_size);
	}

	(*arr)[len] = str;
}

static void rmLine(char *str){
	int i = 0, len = strlen(str);
	for (i = len - 1; i >= 0; i--){
		if (str[i] == '\n'){
			str[i] = '\0';
			break;
		}
	}
}

/*Debug Function*/
void print(char *buf){
	int i = 0, len = strlen(buf) + 1;
	while (i < len){
		if (buf[i] == '\n')
			printf("%s", "\\n");
		else if (buf[i] == '\0')
			printf("%s", "\\0");
		else putchar(buf[i]);
		i++;
	}
	puts("");
}

static char *concat(char *str1, char *str2){
	int prevLen = strlen(str1), newLen = prevLen + strlen(str2) + 1;
	char *tmp = (char *) malloc(newLen * ch_size);
	strcpy(tmp, str1);
	strcpy(tmp + prevLen, str2);
	return tmp;
}

static void clear(char *str, int len){
	int i;
	for (i = 0; i < len; i++)
		str[i] = '\0';
}
