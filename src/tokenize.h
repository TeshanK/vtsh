#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_TOKENS 64
#define MAX_PROCESSES 64

int read_line(char **line);

void tokenize_job(char **line, char **procs, char **infile, char **outfile, int *background);
void tokenize_proc(char *proc, char **args);

char* trim_whitespace(char *str);
int detect_background(char **line);

#endif
