# VTSH - Very Tiny Shell

A simple C shell implementation that executes basic commands.

## Features

- Interactive prompt (`vtsh > `)
- External command execution via fork/execvp
- Space-delimited argument parsing
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
2. Split input into space-delimited tokens
3. Execute commands:
   - For built-ins (`cd`, `exit`): Handle directly in the shell
   - For external commands: Create a child process with fork() and execute with execvp()

## Planned Features

- Pipe support
- Background execution (`&`)
- Command history
- Environment variable expansion
- Redirections (`<`, `>`, `>>`)

## Makefile

The project includes a Makefile with the following features:
- Organized build structure with separate source and build directories
- Compilation with warnings and optimizations (`-Wall -Wextra -O3`)
- Clean target for easy project cleanup

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
vtsh > exit
```

## Author

Teshan Kannangara
