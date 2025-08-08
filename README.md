# VTSH - A Very Tiny Shell

A simple C shell implementation that executes basic commands.

## Features

- **Interactive Command Line**: Uses GNU Readline for a user-friendly prompt.
- **Command History**: Navigate through previous commands using arrow keys.
- **Tab Completion**: Autocompletes file paths and commands.
- **Pipeline Support**: Chain commands together with pipes (`|`).
- **Background Jobs**: Run commands in background using `&`.
- **Job Control**: Proper process group management and signal handling.
- **External Commands**: Executes any command from the system's `PATH` (e.g., `ls`, `grep`).
- **Built-in Commands**:
  - `cd`: Change the current directory.
  - `help`: Display a list of built-in commands.
  - `exit`: Terminate the shell.
- **I/O Redirection**:
  - `>`: Redirect output to a file (overwrite).
  - `<`: Read input from a file.

## Project Structure

```
vtsh/
├── Makefile           # Build configuration
├── README.md          # README file
├── build/             # Compiled executable appears here
│   └── vtsh
└── src/
    ├── shell.c        # Main shell loop and initialization
    ├── shell.h        # Shell global state and functions
    ├── builtins.c     # Built-in command implementations
    ├── builtins.h     # Built-in command headers
    ├── job.c          # Job control and process management
    ├── job.h          # Job control data structures
    ├── tokenize.c     # Input parsing and tokenization
    └── tokenize.h     # Tokenization functions
```

## Building and Running

1.  **Install Dependencies**

    You need `gcc` and `make` to build the shell. You also need the GNU Readline library.

    ```bash
    # For Debian/Ubuntu
    sudo apt-get update
    sudo apt-get install build-essential libreadline-dev

    # For Fedora/RHEL
    sudo dnf groupinstall "Development Tools"
    sudo dnf install readline-devel

    # For Arch Linux
    sudo pacman -S base-devel readline
    ```

2.  **Clone and Build**

    ```bash
    # Clone the repository
    git clone https://github.com/TeshanK/vtsh.git
    cd vtsh

    # Build the shell
    make
    ```

3.  **Run the Shell**

    ```bash
    ./build/vtsh
    ```

4.  **Clean Up**

    ```bash
    # Remove the build directory and executable
    make clean
    ```