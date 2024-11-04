# Word Count Program

This program is a command-line utility to count word frequencies in text files. It can process both single files and directories, allowing for recursive file parsing within directories. The program identifies words based on certain rules and outputs each word alongside its count, sorted by frequency and then alphabetically.

## Features
- **Directory and File Support**: Can process individual text files or all `.txt` files within a directory (including subdirectories).
- **Hash Table**: Efficiently stores word frequencies using a hash table, reducing lookup time for counting word occurrences.
- **Sorting**: Outputs results sorted by word frequency and alphabetically for easier readability.

## Installation
To compile the program, use the provided Makefile:

```bash
make
```

This will produce an executable named `words`.

## Usage
Run the executable with a file or directory as an argument:

```bash
./words <file or directory path>
```

## Code Overview

### 1. `main`
The entry point of the program. It checks each provided argument to determine if it is a file or a directory. For each file, `process_file` is called, while for each directory, `process_directory` is called to recursively parse files.

### 2. `process_directory`
Recursively traverses a directory, calling `process_file` on any `.txt` files found. It skips hidden files and directories.

### 3. `process_file`
Opens a file, reads its content in chunks, and identifies words. It follows specific rules for valid characters (letters, apostrophes, hyphens) and handles hyphenated words carefully. Each identified word is then stored in the hash table.

### 4. `insert_word`
Inserts a new word into the hash table or increments its count if it already exists. It uses chaining to handle hash collisions.

### 5. `output_results`
Retrieves all words from the hash table, sorts them by frequency and lexicographically, and writes the output to standard output.

### Makefile
The Makefile provides rules for building (`make all`) and cleaning (`make clean`) the program. It compiles the source file `words.c` with appropriate flags for warnings and debugging.

## Cleanup
To remove generated files, run:

```bash
make clean
```

