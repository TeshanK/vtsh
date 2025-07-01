/**
 * VTSH - Very Tiny Shell
 * 
 * A minimalistic shell implementation that supports basic command execution.
 * This shell can read user input, parse commands, and execute external programs.
 *
 * Features:
 * - Command prompt interface (vtsh > )
 * - External command execution
 * - Basic tokenization of command arguments
 * - Exit command to terminate the shell
 *
 * File: main.c
 * Author: Teshan Kannangara
 * Date: July 1, 2025
 */

#include <stdio.h>    /* Standard I/O functions */
#include <stdlib.h>   /* Memory allocation, process control */
#include <unistd.h>   /* POSIX operating system API */
#include <string.h>   /* String handling functions */
#include <sys/wait.h> /* Process waiting */

/* Maximum number of tokens (command arguments) supported */
#define NUM_TOKENS 64

/* Function prototypes */
void vtsh_loop(void);                      /* Main shell loop */
int vtsh_read_line(char **, size_t *);     /* Read a line of input */
void vtsh_tokenize_input(char **, char **); /* Parse line into tokens */
int vtsh_run(char **);                     /* Execute the command */

/**
 * Main entry point for the shell
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Exit status of the program
 */
int main(int argc, char **argv)
{
    /* Start the shell's main loop */
    vtsh_loop();

    return 0;
}

/**
 * Main shell loop function
 * 
 * This function implements the read-parse-execute loop that forms the core of the shell:
 * 1. Display prompt and read user input
 * 2. Parse the input into tokens (command and arguments)
 * 3. Execute the command
 * 4. Repeat until exit condition (user types "exit" or EOF)
 */
void vtsh_loop(void)
{
    /* Input buffer pointer and size */
    char *line = NULL;     /* Will hold the user's input string */
    size_t size = 0;       /* Size of allocated buffer */
    int status = 1;        /* Shell status: 1 = continue, 0 = exit */

    /* Main shell loop - continue until status becomes 0 */
    while (status) {
        /* Display the shell prompt */
        printf("vtsh > ");
        fflush(stdout);    /* Ensure prompt is displayed immediately */

        /* Clean up previous input buffer if it exists */
        if (line != NULL) {
            free(line);
            line = NULL;
            size = 0;      /* Critical: reset buffer size for getline */
        }

        /* Read a line of input from the user */
        if (vtsh_read_line(&line, &size) != EXIT_SUCCESS) {
            /* Exit loop on read failure (e.g., EOF) */
            break;
        }

        /* Array to store tokenized command and arguments */
        char *tokens[NUM_TOKENS];

        /* Split the input line into tokens */
        vtsh_tokenize_input(&line, tokens);

        /* Execute the command and check if shell should exit */
        if(vtsh_run(tokens) == EXIT_FAILURE) {
            status = 0;    /* Exit the shell */
        }

        /* Clean up input buffer */
        free(line);
        line = NULL;
    }
}

/**
 * Read a line of input from the user
 * 
 * This function reads a complete line from standard input using getline(),
 * which automatically allocates the appropriate buffer size.
 * It also removes the trailing newline character from the input.
 *
 * @param line Double pointer to the buffer where the line will be stored
 * @param size Pointer to the size of the allocated buffer
 * @return EXIT_SUCCESS on successful read, EXIT_FAILURE otherwise
 */
int vtsh_read_line(char **line, size_t *size)
{
    /* Read a line using getline() which handles dynamic memory allocation */
    if (getline(line, size, stdin) == -1) {
        perror("Reading input failed\n");
        return EXIT_FAILURE;  /* Return failure on EOF or error */
    }

    /* Remove the trailing newline character from input */
    (*line)[strcspn(*line, "\n")] = '\0';

    return EXIT_SUCCESS;
}

/**
 * Tokenize the input line into command and arguments
 * 
 * This function splits the input line into tokens using space as a delimiter.
 * The tokens are stored in the provided array, with the last element set to NULL
 * to match the format required by execvp().
 *
 * @param line Pointer to the input line string
 * @param tokens Array to store the resulting tokens
 */
void vtsh_tokenize_input(char **line, char **tokens)
{
    char *token = NULL;        /* Current token being processed */
    char *delims = " ";        /* Delimiter for tokenization (space) */

    /* Get the first token */
    token = strtok(*line, delims);
    int i = 0;                 /* Token counter */

    /* Process all tokens until end of string or token limit reached */
    while (token && (i < NUM_TOKENS - 1)) {
        tokens[i++] = token;   /* Store token and increment counter */
        token = strtok(NULL, delims);  /* Get next token */
    }

    /* NULL-terminate the token array (required for execvp) */
    tokens[i] = NULL;
}

/**
 * Execute the command specified by the tokens
 * 
 * This function handles both built-in commands (currently only "exit")
 * and external commands. For external commands, it uses fork() and execvp()
 * to create a child process that executes the command.
 *
 * @param args Array of strings containing command and arguments (NULL-terminated)
 * @return EXIT_SUCCESS to continue shell execution, EXIT_FAILURE to exit shell
 */
int vtsh_run(char **args)
{
    /* Check if any command was entered */
    if (args[0] == NULL) {
        return EXIT_SUCCESS;  /* Empty command, continue shell */
    }
    
    /* Handle built-in "exit" command */
    if (strcmp(args[0], "exit") == 0) {
        return EXIT_FAILURE;  /* Signal shell to exit */
    }

    /* For external commands, create a child process */
    pid_t pid;
    pid = fork();  /* Create a new process */

    if (pid < 0) {
        /* Fork failed */
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        /* Child process */
        execvp(args[0], args);  /* Replace process with command */
        
        /* If execvp returns, it means an error occurred */
        perror("command execution failed");
        exit(EXIT_FAILURE);
    } 
    else {
        /* Parent process */
        wait(NULL);  /* Wait for child process to complete */
    }

    return EXIT_SUCCESS;  /* Continue shell execution */
}