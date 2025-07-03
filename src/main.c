/**
 * VTSH - Very Tiny Shell
 * 
 * A simple shell that reads user input, parses commands, and executes programs.
 * 
 * Author: Teshan Kannangara
 * Date: July 1, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define NUM_TOKENS 64  /* Max number of command arguments */

/* Function prototypes */
void vtsh_loop(void);                     
int vtsh_read_line(char **, size_t *);    
void vtsh_tokenize_input(char **, char **);
int vtsh_run(char **);

/* Main entry point */
int main()
{
    printf("\nWelcome to VTSH - Very Tiny Shell\n");
    printf("Type 'help' for information.\n\n");

    vtsh_loop();

    printf("Exiting VTSH. Goodbye!\n");
    
    // Ensure all output is flushed before exiting
    fflush(stdout);
    fflush(stderr);

    return 0;
}

/* Main shell read-parse-execute loop */
void vtsh_loop(void)
{
    char *line = NULL;
    size_t size = 0;
    int status = 1;  /* Shell running status */

    while (status) {
        printf("vtsh > ");
        fflush(stdout);

        /* Clean up previous buffer */
        if (line != NULL) {
            free(line);
            line = NULL;
            size = 0;  /* Reset buffer size for getline */
        }

        if (vtsh_read_line(&line, &size) != 0) {
            break;  /* Exit on read failure (EOF) */
        }

        char *tokens[NUM_TOKENS];
        vtsh_tokenize_input(&line, tokens);

        if(vtsh_run(tokens) == 1) {
            status = 0;  /* Exit shell if command returns failure */
        }

        printf("\n");
        
        free(line);
        line = NULL;
    }
}

/* Read input line from user, removing trailing newline */
int vtsh_read_line(char **line, size_t *size)
{
    if (getline(line, size, stdin) == -1) {
        perror("Reading input failed\n");
        return 1;
    }

    (*line)[strcspn(*line, "\n")] = '\0';  /* Remove trailing newline */

    if(*line == NULL) {
        printf("\n");
    }

    return 0;
}

/* Split input line into tokens (command and arguments) */
void vtsh_tokenize_input(char **line, char **tokens)
{
    char *token = NULL;
    char *delims = " ";

    token = strtok(*line, delims);
    int i = 0;

    while (token && (i < NUM_TOKENS - 1)) {
        tokens[i++] = token;
        token = strtok(NULL, delims);
    }

    tokens[i] = NULL;  /* NULL-terminate for execvp */
}

/* Builtin cd function */
int vtsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "vtsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("vtsh");
        }
    }
    return 0;
}

/* Builtin help function */
void vtsh_help()
{
    printf("\n");
    printf(" 'cd' to navigate to folders.\n");
    printf(" 'help' to get this help message.\n");
    printf(" 'exit' to exit the shell.\n");
    printf(" \nFor help with other commands use 'man' command.\n");
}

/* Execute command (built-in or external) */
int vtsh_run(char **args)
{
    if (args[0] == NULL) {
        return 0;  /* Empty command */
    }
    
    if (strcmp(args[0], "help") == 0) {
        vtsh_help();
        return 0;
    }

    if (strcmp(args[0], "cd") == 0) {
        vtsh_cd(args);
        return 0;
    }

    if (strcmp(args[0], "exit") == 0) {
        return 1;  /* Exit shell */
    }

    
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } 
    else if (pid == 0) {
        /* Child process */
        execvp(args[0], args);
        perror("command execution failed");
        exit(1);
    } 
    else {
        /* Parent process */
        wait(NULL);  /* Wait for child to complete */
    }

    return 0;
}