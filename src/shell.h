#ifndef SHELL_H
#define SHELL_H

#include <termios.h>
#include <sys/types.h>
#include "builtins.h"

extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;

void shell_init(void);
void shell_loop(void);
void shell_cleanup(void);
void print_welcome(void);

void setup_signal_handlers(void);

#endif
