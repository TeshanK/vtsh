#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "shell.h"
#include "job.h"
#include "tokenize.h"
#include <readline/readline.h>
#include <readline/history.h>

/* Global shell state */
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void setup_signal_handlers(void)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
}

void shell_init(void)
{
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        /* Loop until we are in the foreground. */
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp())) {
            kill(-shell_pgid, SIGTTIN);
        }

        setup_signal_handlers();

        /* Put ourselves in our own process group. */
        shell_pgid = getpid();
        if (setpgid(shell_pgid, shell_pgid) < 0) {
            perror("Couldn't put the shell in its own process group");
            exit(1);
        }

        /* Grab control of the terminal. */
        tcsetpgrp(shell_terminal, shell_pgid);

        /* Save default terminal attributes for shell. */
        tcgetattr(shell_terminal, &shell_tmodes);
    }
}

void shell_cleanup(void)
{
    /* Clean up readline */
    rl_clear_history();
    rl_cleanup_after_signal();
    
    /* Reset terminal to original state if needed */
    if (shell_is_interactive) {
        tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
    }
}

void print_welcome(void)
{
    printf("\033[2J\033[H");  /* Clear screen and move cursor to top */
    
    printf("\033[1;35m██╗   ██╗████████╗███████╗██╗  ██╗\033[0m\n");
    printf("\033[1;35m██║   ██║╚══██╔══╝██╔════╝██║  ██║\033[0m\n");
    printf("\033[1;35m██║   ██║   ██║   ███████╗███████║\033[0m\n");
    printf("\033[1;35m╚██╗ ██╔╝   ██║   ╚════██║██╔══██║\033[0m\n");
    printf("\033[1;35m ╚████╔╝    ██║   ███████║██║  ██║\033[0m\n");
    printf("\033[1;35m  ╚═══╝     ╚═╝   ╚══════╝╚═╝  ╚═╝\033[0m\n");
    printf("\n");
    printf("     \033[1;33m✨ A Very Tiny Shell Implementation ✨\033[0m\n");
    printf("    \033[0;37mType '\033[1;93mhelp\033[0;37m' for built-in commands\033[0m\n");
    printf("    \033[0;37mType '\033[1;93mexit\033[0;37m' to quit vtsh\033[0m\n");
    printf("\n");

}

int setup_io_redirection(job_t *j, char *infile, char *outfile)
{
    if (infile != NULL) {
        int fd = open(infile, O_RDONLY);
        if (fd < 0) {
            perror(infile);
            return -1;
        }
        j->stdin = fd;
    } else {
        j->stdin = STDIN_FILENO;
    }

    if (outfile != NULL) {
        int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 
                      S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH);
        if (fd < 0) {
            perror(outfile);
            return -1;
        }
        j->stdout = fd;
    } else {
        j->stdout = STDOUT_FILENO;
    }

    return 0;
}

void shell_loop(void)
{
    int status = 1;

    while (status) {
        char *line = NULL;
        char *procs[MAX_PROCESSES];
        char *infile = NULL;
        char *outfile = NULL;
        int background = 0;

        status = read_line(&line);
        if (!status) break;
        
        if (!line || strlen(line) == 0) {
            if (line) free(line);
            continue;
        }

        job_t *j = create_job();

        tokenize_job(&line, procs, &infile, &outfile, &background);
        j->background = background;

        if (!procs[0]) {
            free_job(j);
            if (line) free(line);
            if (infile) free(infile);
            if (outfile) free(outfile);
            continue;
        }

        /* Check for builtin commands first (only if single command, no pipes) */
        int is_builtin_cmd = 0;
        if (procs[0] && !procs[1] && !background) {
            char **proc_args = (char **)malloc(MAX_TOKENS * sizeof(char *));
            if (!proc_args) {
                perror("malloc failed for process arguments");
                free_job(j);
                exit(1);
            }
            
            tokenize_proc(procs[0], proc_args);
            
            if (proc_args[0] && is_builtin(proc_args[0])) {
                int result = execute_builtin(proc_args);
                if (result == 1) {  /* exit command */
                    status = 0;  /* Exit shell loop */
                }
                is_builtin_cmd = 1;
            }

            for (int i = 0; proc_args[i]; i++) {
                free(proc_args[i]);
            }
            free(proc_args);
        }

        if (is_builtin_cmd) {
            free_job(j);
            if (line) free(line);
            if (infile) free(infile);
            if (outfile) free(outfile);
            continue;
        }

        if (setup_io_redirection(j, infile, outfile) < 0) {
            free_job(j);
            if (line) free(line);
            if (infile) free(infile);
            if (outfile) free(outfile);
            continue;
        }

        int i = 0;
        while (procs[i]) {
            char **proc_args = (char **)malloc(MAX_TOKENS * sizeof(char *));
            if (!proc_args) {
                perror("malloc failed for process arguments");
                free_job(j);
                exit(1);
            }

            tokenize_proc(procs[i], proc_args);

            if (!proc_args[0]) {
                free(proc_args);
                i++;
                continue;
            }

            process_t *p = create_process(proc_args);
            add_process(p, j);
            i++;
        }

        if (j->first) {
            j->cmd = strdup(line);
            launch_job(j);
        }

        /* Close file descriptors if they are non standard */
        if (j->stdin != STDIN_FILENO) close(j->stdin);
        if (j->stdout != STDOUT_FILENO) close(j->stdout);

        free_job(j);
        if (line) free(line);

        if (infile) free(infile);
        if (outfile) free(outfile);
    }
}
int main(void)
{
    /* Register cleanup function */
    atexit(shell_cleanup);
    
    shell_init();
    print_welcome();
    shell_loop();

    return 0;
}
