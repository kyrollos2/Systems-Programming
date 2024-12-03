# My Shell (mysh) - Project README

Welcome to the **My Shell (mysh)** project! This project involves designing and implementing a simple command-line shell similar to `bash` or `zsh`. Below are the details about the project, how it works, and the testing strategy.

---

## Project Overview

**mysh** is a shell program capable of running in two modes:  
- **Interactive mode**: Provides prompts, welcome/goodbye messages, and error status notifications.  
- **Batch mode**: Reads commands from a file or standard input without prompts or additional output.  

Key features implemented in this project include:
- Command execution with argument parsing.
- File redirection for standard input and output.
- Pipeline support for chaining two processes.
- Built-in commands: `cd`, `pwd`, `which`, and `exit`.
- Wildcard expansion for tokens containing `*`.

---

## Features and Functionalities

1. **Interactive Mode**  
   Displays a welcome message (`"Welcome to my shell!"`) and provides a prompt (`mysh> `) for user input. Displays goodbye message (`"Exiting my shell."`) on termination.

2. **Batch Mode**  
   Executes commands from a specified file or piped input without any prompts or messages.

3. **Built-in Commands**  
   - **`cd`**: Changes the working directory.
   - **`pwd`**: Prints the current working directory.
   - **`which`**: Identifies the executable path for a program.
   - **`exit`**: Terminates the shell with an optional message.

4. **File Redirection**  
   - `<`: Redirects standard input.
   - `>`: Redirects standard output with truncation or creation of files.

5. **Pipelines**  
   Supports a single pipeline (e.g., `command1 | command2`) with proper redirection.

6. **Wildcard Expansion**  
   Expands patterns containing `*` to match files in the current or specified directory.

7. **Command Execution**  
   Executes programs using `execv()` and supports error handling for non-zero exit codes or signal termination.

---

## Files in the Repository

- **`mysh.c`**: Contains the source code for the shell implementation.
- **`Makefile`**: Automates the build process for the project.
- **`README.md`**: This documentation file.
- **Test Files**:
  - `test_script.sh`: Example batch mode script for testing.
  - Various input files for redirection and wildcard tests.

---

## Compilation and Usage

### Compilation
Run the following command to compile the program:
```bash
make
```

### Usage
1. **Interactive Mode**:  
   Start the shell without arguments:  
   ```bash
   ./mysh
   ```
   Example:  
   ```bash
   Welcome to my shell!
   mysh> echo Hello, World!
   Hello, World!
   mysh> exit
   Exiting my shell.
   ```

2. **Batch Mode**:  
   Start the shell with a script file or piped input:  
   ```bash
   ./mysh script_file.sh
   ```
   Example:  
   ```bash
   $ echo "echo Hello, Batch Mode!" > script.sh
   $ ./mysh script.sh
   Hello, Batch Mode!
   ```

---

## Testing Strategy

### Test Plan
1. **Basic Command Execution**  
   Test single commands with and without arguments (e.g., `ls`, `echo`, etc.).  

2. **Built-in Commands**  
   - Change directories with `cd` and verify with `pwd`.  
   - Use `which` for valid and invalid commands.  
   - Test the `exit` command with and without arguments.  

3. **Redirection**  
   - Test `<` and `>` with valid and invalid files.  
   - Verify behavior when files cannot be opened.  

4. **Pipelines**  
   - Test single pipelines (e.g., `ls | grep .c`).  
   - Verify standard input/output redirection in pipelines.  

5. **Wildcard Expansion**  
   - Match files in the current directory with patterns like `*.txt`.  
   - Test with no matching files to ensure tokens are passed unchanged.  

6. **Interactive Mode**  
   - Check welcome/goodbye messages.  
   - Test prompts and status messages for abnormal exits.  

7. **Batch Mode**  
   - Run predefined scripts and compare outputs to expected results.

### Test Cases
1. Commands with and without arguments:
   ```bash
   echo Hello
   pwd
   ```
2. File redirection:
   ```bash
   cat < input.txt > output.txt
   ```
3. Pipelines:
   ```bash
   ls | grep .c
   ```
4. Wildcard expansions:
   ```bash
   echo *.txt
   ```
5. Built-in commands:
   ```bash
   cd subdir
   which ls
   exit
   ```

### Test Execution
Run the following command for batch mode tests:
```bash
./mysh < test_script.sh
```

---

## Authors

- **Neer Patel**  
- **Kyro**

NetIDs:
- **[Your NetID]**
- **[Partner's NetID]**

---

## Notes

This project provides an excellent introduction to systems programming concepts such as process management, file I/O, and inter-process communication in Unix-like environments.  

Feel free to report any issues or contribute enhancements!
