// main.c

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "parser.h"
#include "executor.h"
#include "utils.h"

#define BUFFER_SIZE 1024

void command_loop(int input_fd, int interactive_mode);

int main(int argc, char *argv[]) {
    int interactive_mode = isatty(STDIN_FILENO);
    int input_fd = STDIN_FILENO;  // Default to standard input

    // Handle command-line arguments
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        // Open batch file
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd < 0) {
            perror("Failed to open batch file");
            exit(EXIT_FAILURE);
        }
        interactive_mode = 0;  // Override interactive mode
    }

    // Determine if input is from a terminal
    if (interactive_mode) {
        printf("Welcome to my shell!\n");
    }

    // Start the command loop
    command_loop(input_fd, interactive_mode);

    if (interactive_mode) {
        printf("Exiting my shell.\n");
    }

    // Close batch file if opened
    if (input_fd != STDIN_FILENO) {
        close(input_fd);
    }

    return EXIT_SUCCESS;
}

void command_loop(int input_fd, int interactive_mode) {
    char *input_line = NULL;
    size_t line_size = 0;
    ssize_t nread;
    FILE *input_stream = fdopen(input_fd, "r");

    if (!input_stream) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Display prompt in interactive mode
        if (interactive_mode) {
            printf("mysh> ");
            fflush(stdout);
        }

        // Read input using getline()
        nread = getline(&input_line, &line_size, input_stream);
        if (nread == -1) {
            // End of input (EOF) or error
            if (feof(input_stream)) {
                break;
            } else {
                perror("getline");
                exit(EXIT_FAILURE);
            }
        }

        // Remove the newline character at the end
        if (nread > 0 && input_line[nread - 1] == '\n') {
            input_line[nread - 1] = '\0';
        }

        // Trim whitespace from the input line
        char *trimmed_line = trim_whitespace(input_line);
        if (strlen(trimmed_line) == 0) {
            // Empty command, skip
            continue;
        }

        // Tokenize the input line
        char **tokens = tokenize_input(trimmed_line);
        if (!tokens) {
            continue;
        }

        // Parse the tokens into a command structure
        struct command *cmd = parse_command(tokens);
        if (!cmd) {
            free_tokens(tokens);
            continue;
        }

        // Check for exit command
        if (cmd->is_exit) {
            // Print any arguments provided with exit
            if (cmd->argc > 1) {
                for (int i = 1; i < cmd->argc; i++) {
                    printf("%s ", cmd->argv[i]);
                }
                printf("\n");
            }
            free_command(cmd);
            free_tokens(tokens);
            break;
        }

        // Execute the command
        execute_command(cmd, interactive_mode);

        // Free allocated memory
        free_command(cmd);
        free_tokens(tokens);
    }

    free(input_line);
    fclose(input_stream);
}
