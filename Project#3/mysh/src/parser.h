// parser.h

#ifndef PARSER_H
#define PARSER_H

#define MAX_TOKENS 128
#define BUFFER_SIZE 1024

// Command structure
struct command {
    char *cmd_name;       // Command name
    char **argv;          // Argument list
    int argc;             // Argument count
    char *input_file;     // Input redirection file
    char *output_file;    // Output redirection file
    int is_builtin;       // Flag for built-in commands
    int is_exit;          // Flag for exit command
    int is_pipe;          // Flag for pipe
    struct command *next; // Next command in pipeline
};

// Function declarations
char **tokenize_input(const char *input_line);
struct command *parse_command(char **tokens);
void free_tokens(char **tokens);
void free_command(struct command *cmd);

#endif // PARSER_H
