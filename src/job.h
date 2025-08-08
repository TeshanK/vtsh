#ifndef JOB_H
#define JOB_H

#include <sys/types.h>
#include <termios.h>

#define MAX_PIPELINE_SIZE 64

typedef struct process_t {
    char **args;                /* Command arguments */
    pid_t pid;                  /* Process ID */
    struct process_t *next;     /* Next process in pipeline */
} process_t;

typedef struct job_t {
    struct process_t *first;    /* First process in pipe */
    char *cmd;                  /* Command string */
    pid_t pgid;                 /* Process group ID */
    struct termios tmodes;      /* Terminal modes */
    int stdin, stdout;          /* I/O redirections */
    int background;             /* 1 if background job, 0 if foreground */
} job_t;

job_t* create_job(void);
void add_process(process_t *p, job_t *j);
void free_job(job_t *j);
int num_processes(job_t *j);

process_t* create_process(char **args);
void free_processes(job_t *j);

void launch_job(job_t *j);
void launch_process(process_t *p, pid_t pgid, int infile, int outfile);
void wait_for_job(job_t *j);
void put_job_in_foreground(job_t *j);

#endif
