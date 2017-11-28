#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 512
extern const size_t ptr_size;

char *input();
char **tokenize(char *str, const char *delim, int *num_tok, int *size);
void append(char *str, int len, char ***arr, int *size);
void print(char *buf);

#endif
