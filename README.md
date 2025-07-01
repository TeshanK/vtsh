# VTSH - Very Tiny Shell

A simple C shell implementation that executes basic commands.

## Features

- Interactive prompt (`vtsh > `)
- External command execution via fork/execvp
- Space-delimited argument parsing
- Built-in `exit` command

## Project Structure

```
vtsh/
├── README.md
└── src/
    ├── main.c
    └── vtsh
```

## How It Works

1. Read input from user
2. Split input into space-delimited tokens
3. Execute built-in or external commands

## Planned Features

- Expansions (brace, parameter, arithmetic, etc.)
- Quote handling
- Redirections (`<`, `>`, `>>`)

## Building and Running

```bash
cd vtsh/src
gcc -o vtsh main.c
./vtsh
```

## Usage

```
vtsh > ls -la
vtsh > pwd
vtsh > exit
```

## Author

Teshan Kannangara
