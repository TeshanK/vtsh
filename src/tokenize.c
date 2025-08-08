#define _POSIX_C_SOURCE 200809L
#include "tokenize.h"

/* Read input line from user */
int read_line(char **line)
{
    *line = readline("\n\033[1;36mvtsh\033[0;94m> \033[0m");

    if (*line == NULL) {
        printf("\n");
        return 0;
    }

    /* Empty line */
    if (strlen(*line) == 0) {
        free(*line);
        *line = NULL;
        return 1;
    }

    /* Add to history */
    add_history(*line);
    return 1;
}

char* trim_whitespace(char *str)
{
    char *end;

    /* Trim leading space */
    while (*str == ' ' || *str == '\t') str++;

    if (*str == 0) return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;

    *(end + 1) = '\0';

    return str;
}

int detect_background(char **line)
{
    if (!*line || strlen(*line) == 0) return 0;

    char *trimmed = trim_whitespace(*line);
    int len = strlen(trimmed);

    if (len > 0 && trimmed[len - 1] == '&') {
        trimmed[len - 1] = '\0';
        trimmed = trim_whitespace(trimmed);

        strcpy(*line, trimmed);
        return 1;
    }

    return 0;
}

void parse_redirections(char *token, char **infile, char **outfile, char **cmd_part)
{
    if (!token) return;

    char *redirect_part_in = strdup(token);
    char *redirect_part_out = strdup(token);
    *cmd_part = strtok(token, "<>");

    /* Parse input redirection */
    char *input_pos = strstr(redirect_part_in, "<");
    if (input_pos != NULL) {
        input_pos++;
        input_pos = trim_whitespace(input_pos);
        char *input_end = strpbrk(input_pos, "> \n");
        if (input_end != NULL) *input_end = '\0';
        *infile = strdup(input_pos);
    }

    /* Parse output redirection */
    char *output_pos = strstr(redirect_part_out, ">");
    if (output_pos != NULL) {
        output_pos++;
        output_pos = trim_whitespace(output_pos);
        char *output_end = strpbrk(output_pos, "< \n");
        if (output_end != NULL) *output_end = '\0';
        *outfile = strdup(output_pos);
    }

    free(redirect_part_in);
    free(redirect_part_out);
}

void tokenize_job(char **line, char **procs, char **infile, char **outfile, int *background)
{
    char *token = NULL;
    const char *job_delim = "|";
    int i = 0;

    *background = 0;
    *infile = NULL;
    *outfile = NULL;

    /* Handle empty line */
    if (!*line || strlen(*line) == 0) {
        procs[0] = NULL;
        return;
    }

    *background = detect_background(line);

    token = strtok(*line, job_delim);
    while (token && i < MAX_PROCESSES - 1) {
        procs[i++] = token;
        token = strtok(NULL, job_delim);
    }

    if (i == 0) {
        procs[0] = NULL;
        return;
    }

    char *last_token = procs[i - 1];
    if (last_token && (strstr(last_token, "<") || strstr(last_token, ">"))) {
        char *cmd_part;
        parse_redirections(last_token, infile, outfile, &cmd_part);
        procs[i - 1] = cmd_part;
    }

    procs[i] = NULL;
}

void tokenize_proc(char *proc, char **args)
{
    char *token = NULL;
    int i = 0;

    if (!proc) {
        args[0] = NULL;
        return;
    }

    /* Make a copy of the string since strtok modifies it */
    char *proc_copy = strdup(proc);
    if (!proc_copy) {
        args[0] = NULL;
        return;
    }

    char *trimmed = trim_whitespace(proc_copy);

    token = strtok(trimmed, " \t");
    while (token && i < MAX_TOKENS - 1) {
        args[i] = strdup(token);
        if (!args[i]) {
            for (int j = 0; j < i; j++) {
                free(args[j]);
            }
            free(proc_copy);
            args[0] = NULL;
            return;
        }
        i++;
        token = strtok(NULL, " \t");
    }

    args[i] = NULL;  /* Null-terminate for exec */

    free(proc_copy);
}
