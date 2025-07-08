/**
 * VTSH - Very Tiny Shell
 * 
 * A simple shell that reads user input, parses commands, and executes programs.
 * 
 * Author: Teshan Kannangara
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define NUM_TOKENS 64  /* Max number of command arguments */

/* Function prototypes */
void vtsh_loop(void);                     
int vtsh_read_line(char **);    
int vtsh_tokenize_input(char **, char **, int *, int *);
int vtsh_launch(char **, int, int);
int vtsh_run(char **, int, int);

/* Built-in commands */
int vtsh_cd(char **);
int vtsh_help(char **);
int vtsh_exit(char **);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &vtsh_cd,
    &vtsh_help,
    &vtsh_exit
};

int vtsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

void print_welcome()
{
    printf("\nWelcome to VTSH - Very Tiny Shell\n");
    printf("Type 'help' for information.\n\n");
}

/* Builtin cd function */
int vtsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "vtsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("vtsh");
            return -1;
        }
    }
    return 0;
}

/* Builtin exit function */
int vtsh_exit(char **)
{
    printf("Exiting VTSH. Goodbye!\n");
    return 1;
}

/* Builtin help function */
int vtsh_help(char **)
{
    printf("\n");
    printf(" 'cd' to navigate to folders.\n");
    printf(" 'help' to get this help message.\n");
    printf(" 'exit' to exit the shell.\n");
    printf(" \nFor help with other commands use 'man' command.\n");
    return 0;
}

/* Main entry point */
int main()
{
    // Auto-complete paths when the tab key is hit
    rl_bind_key('\t', rl_complete);
    
    // Enable command history
    using_history();

    print_welcome();

    vtsh_loop();
    
    // Ensure all output is flushed before exiting
    fflush(stdout);
    fflush(stderr);

    return 0;
}

/* Main shell read-parse-execute loop */
void vtsh_loop(void)
{
    char *line = NULL;
    int status = 1;  /* Shell running status */
    int output_redirect = STDOUT_FILENO;  /* File descriptor for output redirection */
    int input_redirect = STDIN_FILENO;  /* File descriptor for input redirection */

    while (status) {
        fflush(stdout);

        /* Clean up previous buffer */
        if (line != NULL) {
            free(line);
            line = NULL;
        }

        if (vtsh_read_line(&line) != 0) {
            break;  /* Exit on read failure (EOF) */
        }

        char *tokens[NUM_TOKENS];

        if(vtsh_tokenize_input(&line, tokens, &output_redirect, &input_redirect) == 0) {
            if(vtsh_run(tokens, output_redirect, input_redirect) == 1) {
                status = 0;  /* Exit shell if command returns failure */
            }
        }

        printf("\n");

        if (output_redirect != STDOUT_FILENO) {
            close(output_redirect);
            output_redirect = STDOUT_FILENO;
        }

        if (input_redirect != STDIN_FILENO) {
            close(input_redirect);
            input_redirect = STDIN_FILENO;
        }

        free(line);
        line = NULL;
    }
}

/* Read input line from user, removing trailing newline */
int vtsh_read_line(char **line)
{
    *line = readline("vtsh> ");
    if (*line == NULL) {
        perror("Reading input failed\n");
        return 1;
    }

    (*line)[strcspn(*line, "\n")] = '\0';  /* Remove trailing newline */

    if(*line == NULL) {
        printf("\n");
    } else {
        // Add input to readline history
        add_history(*line);
    }

    return 0;
}

/* Split input line into tokens (command and arguments) */
int vtsh_tokenize_input(char **line, char **tokens, int *output_redirect, int *input_redirect)
{
    char *token = NULL;
    char *delims = " ";

    token = strtok(*line, delims);
    int i = 0;

    while (token && (i < NUM_TOKENS - 1)) {
        if ((strcmp(token, ">") == 0) | (strcmp(token, ">>") == 0)) {
            /* Output redirection */

            int oflag = O_TRUNC; // overwrite output redirection
            if (strcmp(token, ">>") == 0) {
                oflag = O_APPEND; // appending output redirection
            }
            
            token = strtok(NULL, delims);

            if(token != NULL){
                int fd = open(token, O_CREAT|O_WRONLY|oflag, 0644); // Owner read/write, group read, others read
                if (fd == -1) {
                    fprintf(stderr, "failed to open file %s\n", token);
                    return 1;
                } else {
                    *output_redirect = fd;
                }
                break;
            } else {
                fprintf(stderr, "vtsh: please specify the file to redirect output\n");
                return 1;
            }
        } else if (strcmp(token, "<") == 0) {
            /* Input redirection */

            token = strtok(NULL, delims);

            if (token != NULL) {
                int fd = open(token, O_RDONLY);
                if (fd == -1) {
                    fprintf(stderr, "failed to open file %s\n", token);
                    return 1;
                } else {
                    *input_redirect = fd;
                }
                break;
            } else {
                fprintf(stderr, "vtsh: please specify the file to redirect input from\n");
                return 1;
            }
        }
        tokens[i++] = token;
        token = strtok(NULL, delims);
    }

    tokens[i] = NULL;  /* NULL-terminate for execvp */
    return 0;
}

/* Launch non-builtin programs*/
int vtsh_launch(char **args, int output_redirect, int input_redirect)
{
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } 
    else if (pid == 0) {
        /* Child process */

        dup2(output_redirect, STDOUT_FILENO);
        dup2(input_redirect, STDIN_FILENO);

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

/* Execute command (built-in or external) */
int vtsh_run(char **args, int output_redirect, int input_redirect)
{
    if (args[0] == NULL) {
        return 0; /* Empty command */
    }

    /* Run builtin commands*/
    for (int i = 0; i < vtsh_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0) {

            int original_stdout = STDOUT_FILENO;
            int original_stdin = STDIN_FILENO;
            int result = 0;


            if (output_redirect != STDOUT_FILENO) {

                /* Save the original stdout file descriptor to be restored later */
                original_stdout = dup(STDOUT_FILENO);

                /* Redirect stdout to the file */
                dup2(output_redirect, STDOUT_FILENO);
            }

            if (input_redirect != STDIN_FILENO) {

                /* Save the original stdin file descriptor to be restored later */
                original_stdin = dup(STDIN_FILENO);

                /* Redirect stdin to the file */
                dup2(input_redirect, STDIN_FILENO);
            }

            result = (*builtin_func[i])(args);

            /* Restore the original stdout*/
            if (output_redirect != STDOUT_FILENO) {
                dup2(original_stdout, STDOUT_FILENO);
                close(output_redirect);
            }

            /* Restore the original stdin */
            if (input_redirect != STDIN_FILENO) {
                dup2(original_stdin, STDIN_FILENO);
                close(input_redirect);
            }

            return result;
        }
    }

    /* Run external commands*/
    return vtsh_launch(args, output_redirect, input_redirect);
}
