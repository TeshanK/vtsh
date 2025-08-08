#ifndef BUILTINS_H
#define BUILTINS_H

typedef struct {
    char builtin_name[1024];
    int (*builtin_func)(char **);
} shell_builtin_t;

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

extern shell_builtin_t builtins[];
int shell_num_builtins(void);
int is_builtin(const char *cmd);
int execute_builtin(char **args);

#endif
