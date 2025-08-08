#define _POSIX_C_SOURCE 200809L
#include "job.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

job_t* create_job(void)
{
    job_t *j = (job_t *)malloc(sizeof(job_t));
    if (!j) {
        perror("malloc failed for job");
        exit(1);
    }
    
    j->first = NULL;
    j->cmd = NULL;
    j->pgid = 0;
    j->stdin = STDIN_FILENO;
    j->stdout = STDOUT_FILENO;
    j->background = 0;
    
    return j;
}

process_t* create_process(char **args)
{
    process_t *p = (process_t *)malloc(sizeof(process_t));
    if (!p) {
        perror("malloc failed for process");
        exit(1);
    }
    
    p->args = args;
    p->pid = 0;
    p->next = NULL;
    
    return p;
}

void add_process(process_t *p, job_t *j)
{
    if (j->first == NULL) {
        j->first = p;
        return;
    }

    process_t *curr = j->first;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = p;
}

int num_processes(job_t *j)
{
    int count = 0;
    process_t *curr = j->first;

    while (curr) {
        count++;
        curr = curr->next;
    }
    return count;
}

void free_processes(job_t *j)
{
    if (!j || !j->first) return;

    process_t *curr = j->first;
    while (curr) {
        process_t *next = curr->next;
        if (curr->args) {
            for (int i = 0; curr->args[i]; i++) {
                free(curr->args[i]);
            }
            free(curr->args);
        }
        free(curr);
        curr = next;
    }
    j->first = NULL;
}

void free_job(job_t *j)
{
    if (!j) return;
    
    free_processes(j);
    if (j->cmd) {
        free(j->cmd);
    }
    free(j);
}

void wait_for_job(job_t *j)
{
    int status;
    
    do {
        waitpid(-j->pgid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
}

void put_job_in_foreground(job_t *j)
{
    /* Put the job into the foreground. */
    tcsetpgrp(shell_terminal, j->pgid);

    /* Wait for it to complete. */
    wait_for_job(j);

    /* Put the shell back in the foreground. */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Restore the shell's terminal modes. */
    tcgetattr(shell_terminal, &j->tmodes);
    tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

void launch_process(process_t *p, pid_t pgid, int infile, int outfile)
{
    pid_t pid;

    if (shell_is_interactive) {
        /* Put the process into the process group. */
        pid = getpid();
        if (pgid == 0) pgid = pid;
        setpgid(pid, pgid);

        /* Restore default signal handlers. */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }

    /* Set up I/O redirection. */
    if (infile != STDIN_FILENO) {
        dup2(infile, STDIN_FILENO);
        close(infile);
    }

    if (outfile != STDOUT_FILENO) {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }

    /* Execute the command. */
    if (p->args && p->args[0]) {
        execvp(p->args[0], p->args);
        fprintf(stderr, "execvp error for command '%s': ", p->args[0]);
        perror("");
        exit(1);
    } else {
        fprintf(stderr, "Error: process arguments are NULL\n");
        exit(1);
    }
}

void launch_job(job_t *j)
{
    int mypipe[2];
    int infile, outfile;
    pid_t pid;
    
    infile = j->stdin;
    int foreground = !j->background;

    process_t *p;
    for (p = j->first; p; p = p->next) {
        /* Set up pipes. */
        if (p->next) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                exit(1);
            }
            outfile = mypipe[1];
        } else {
            outfile = j->stdout;
        }

        /* Fork a child process. */
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /* Child process. */
            launch_process(p, j->pgid, infile, outfile);
        } else {
            /* Parent process. */
            p->pid = pid;

            if (shell_is_interactive) {
                if (!j->pgid) {
                    j->pgid = pid;
                }
                setpgid(pid, j->pgid);
            }
        }

        /* Clean up after pipes. */
        if (infile != j->stdin) {
            close(infile);
        }
        if (outfile != j->stdout) {
            close(outfile);
        }
        infile = mypipe[0];
    }

    if (!shell_is_interactive) {
        wait_for_job(j);
    } else if (foreground) {
        put_job_in_foreground(j);
    } else {
        /* Background job - print job info and don't wait. */
        printf("[&] %d\n", j->pgid);
    }
}
