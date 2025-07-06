# VTSH - Very Tiny Shell

A simple C shell implementation that executes basic commands.

## Features

- Interactive prompt (`vtsh > `)
- External command execution via fork/execvp
- Space-delimited argument parsing
- Input/output redirection with `<`, `>`, and `>>` operators
- Built-in commands:
  - `exit` - Terminates the shell
  - `cd` - Changes directory
  - `help` - Displays help message
- Simple build system with Makefile

## Project Structure

```
vtsh/
├── Makefile        # Build configuration
├── README.md       # Documentation
├── build/          # Generated executable (after make)
│   └── vtsh
└── src/
    └── main.c      # Source code
```

## How It Works

1. Read input from user
2. Split input into space-delimited tokens and process redirection operators (`<`, `>`, `>>`)
3. Execute commands:
   - For built-ins (`cd`, `exit`, `help`): Handle directly in the shell
   - For external commands: Create a child process with fork() and execute with execvp()
4. Handle redirections:
   - Input redirection (`<`): Read command input from a file
   - Output redirection (`>`): Write command output to a file (overwrite)
   - Append redirection (`>>`): Append command output to a file

## Planned Features

- Pipe support
- Background execution (`&`)
- Command history
- Environment variable expansion

## Building and Running

Using the Makefile:

```bash
# Build the shell
make

# Run the shell
./build/vtsh

# Clean build files
make clean
```

## Usage

```
vtsh > ls -la
vtsh > pwd
vtsh > cd /home
vtsh > ls -l > files.txt       # Redirect output to file (overwrite)
vtsh > echo hello >> files.txt # Append output to file
vtsh > help > help.txt         # Built-in commands support redirection too
vtsh > sort < data.txt         # Read input from file
vtsh > exit
```