# VTSH - Very Tiny Shell

A minimalistic shell implementation written in C that provides basic command execution functionality.

## Overview

VTSH is a simple shell that implements core shell functionality including:
- Interactive command prompt
- Command execution via fork/exec
- Basic command-line parsing
- Built-in commands

## Current Features

- Interactive prompt (`vtsh > `)
- Command execution using `execvp`
- Argument tokenization (space-delimited)
- Built-in `exit` command
- Proper memory management

## Project Structure

```
vtsh/
├── README.md          # Project documentation
└── src/
    ├── main.c         # Main shell implementation
    └── vtsh           # Compiled executable
```

## Shell Processing Pipeline

The shell follows this processing pipeline:

1. **Tokenization**:
   - Split input into tokens based on spaces

2. **Command Identification**:
   - Identify built-in commands vs external commands

3. **Execution**:
   - For built-ins: Handle directly in the shell
   - For externals: Fork and execute with execvp

## Planned Features

These features are planned for future implementation:

1. **Expansion**:
   - [ ] **Stage 1**: Brace Expansion
   - [ ] **Stage 2**:
     - [ ] Parameter Expansion
     - [ ] Arithmetic Expansion
     - [ ] Command Substitution
     - [ ] Tilde Expansion
   - [ ] **Stage 3**: Word Splitting
   - [ ] **Stage 4**: Globbing (Pathname Expansion)

2. **Quote Handling**:
   - [ ] Quote removal
   - [ ] Handling of quoted strings

3. **Redirections**:
   - [ ] Input redirection (`<`)
   - [ ] Output redirection (`>`)
   - [ ] Append redirection (`>>`)

## Building and Running

```bash
# Navigate to source directory
cd vtsh/src

# Compile the shell
gcc -o vtsh main.c

# Run the shell
./vtsh
```

## Usage

Once running, VTSH provides a prompt where you can enter commands:

```
vtsh > ls -la
vtsh > pwd
vtsh > exit
```

## License

This project is open source.

## Author

Teshan Kannangara
