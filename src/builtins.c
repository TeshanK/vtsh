#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"

/* Builtin cd function */
int shell_cd(char **args)
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

/* Builtin help function */
int shell_help(char **args)
{
    (void)args; /* Suppress unused parameter warning */
    
    printf("\nType 'cd <directory>' to change the current directory.\n");
    printf("Type 'exit' to quit vtsh.\n");
    printf("Type 'help' for this help message.\n");
    
    return 0;
}

/* Builtin exit function */
int shell_exit(char **args)
{
    (void)args; /* Suppress unused parameter warning */
    
    printf("\nExiting vtsh. Goodbye!\n");
    return 1;
}

shell_builtin_t builtins[] = {
    {"cd", &shell_cd},
    {"help", &shell_help},
    {"exit", &shell_exit}
};

int shell_num_builtins(void)
{
    return sizeof(builtins) / sizeof(shell_builtin_t);
}

/* Check if a command is a builtin */
int is_builtin(const char *cmd)
{
    if (!cmd) return 0;
    
    for (int i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(cmd, builtins[i].builtin_name) == 0) {
            return 1;
        }
    }
    
    return 0;
}

/* Execute a builtin command */
int execute_builtin(char **args)
{
    if (!args || !args[0]) {
        return 0;
    }
    
    for (int i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtins[i].builtin_name) == 0) {
            return builtins[i].builtin_func(args);
        }
    }
    
    return 0;
}
