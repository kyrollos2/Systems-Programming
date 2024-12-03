// parser.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"
#include "wildcard.h"
#include "utils.h"

#define SPECIAL_CHARS "<>|*"

// parser.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"
#include "wildcard.h"
#include "utils.h"

#define SPECIAL_CHARS "<>|*"

char **tokenize_input(const char *input_line) {
    char **tokens = safe_malloc(sizeof(char *) * MAX_TOKENS);
    int token_count = 0;
    const char *ptr = input_line;
    int in_quotes = 0;
    char quote_char = '\0';
    char buffer[BUFFER_SIZE];
    int buf_index = 0;

    while (*ptr != '\0') {
        if (!in_quotes && isspace((unsigned char)*ptr)) {
            // End of token
            if (buf_index > 0) {
                buffer[buf_index] = '\0';
                tokens[token_count++] = safe_strdup(buffer);
                buf_index = 0;
            }
            ptr++;
            continue;
        }

        if (*ptr == '"' || *ptr == '\'') {
            if (in_quotes && *ptr == quote_char) {
                // Closing quote
                in_quotes = 0;
                quote_char = '\0';
            } else if (!in_quotes) {
                // Opening quote
                in_quotes = 1;
                quote_char = *ptr;
            } else {
                // Inside quotes, treat quote as regular character
                buffer[buf_index++] = *ptr;
            }
            ptr++;
            continue;
        }

        if (in_quotes) {
            // Inside quotes, accumulate characters
            buffer[buf_index++] = *ptr++;
        } else if (strchr(SPECIAL_CHARS, *ptr)) {
            // Handle special characters as separate tokens
            if (buf_index > 0) {
                buffer[buf_index] = '\0';
                tokens[token_count++] = safe_strdup(buffer);
                buf_index = 0;
            }
            tokens[token_count++] = strndup(ptr, 1);
            ptr++;
        } else {
            // Regular character
            buffer[buf_index++] = *ptr++;
        }

        // Prevent buffer overflow
        if (buf_index >= BUFFER_SIZE - 1) {
            fprintf(stderr, "Token too long\n");
            free_tokens(tokens);
            return NULL;
        }

        // Prevent too many tokens
        if (token_count >= MAX_TOKENS - 1) {
            fprintf(stderr, "Too many tokens\n");
            free_tokens(tokens);
            return NULL;
        }
    }

    // Add the last token if any
    if (buf_index > 0) {
        buffer[buf_index] = '\0';
        tokens[token_count++] = safe_strdup(buffer);
    }

    tokens[token_count] = NULL;  // Null-terminate the token array

    // **Debugging: Print the tokens**
    printf("Debug: Tokenization Result:\n");
    for (int i = 0; i < token_count; i++) {
        printf("  Token %d: '%s'\n", i, tokens[i]);
    }

    return tokens;
}


struct command *parse_command(char **tokens) {
    if (!tokens || !tokens[0]) {
        return NULL;
    }

    struct command *cmd = calloc(1, sizeof(struct command));
    if (!cmd) {
        perror("calloc");
        return NULL;
    }

    cmd->argv = safe_malloc(sizeof(char *) * MAX_TOKENS);

    int argc = 0;
    int token_index = 0;
    int in_redirection = 0;
    int out_redirection = 0;
    struct command *current_cmd = cmd;

    while (tokens[token_index]) {
        char *token = tokens[token_index];

        if (strcmp(token, "<") == 0) {
            // Input redirection
            in_redirection = 1;
        } else if (strcmp(token, ">") == 0) {
            // Output redirection
            out_redirection = 1;
        } else if (strcmp(token, "|") == 0) {
            // Pipe
            current_cmd->is_pipe = 1;
            current_cmd->argc = argc;
            current_cmd->argv[argc] = NULL;

            // Allocate new command for the next part of the pipeline
            struct command *next_cmd = calloc(1, sizeof(struct command));
            if (!next_cmd) {
                perror("calloc");
                free_command(cmd);
                return NULL;
            }
            next_cmd->argv = safe_malloc(sizeof(char *) * MAX_TOKENS);
            current_cmd->next = next_cmd;
            current_cmd = next_cmd;
            argc = 0;
        } else if (in_redirection) {
            // Input file
            current_cmd->input_file = safe_strdup(token);
            in_redirection = 0;
        } else if (out_redirection) {
            // Output file
            current_cmd->output_file = safe_strdup(token);
            out_redirection = 0;
        } else {
            // Handle wildcards
            char **expanded_tokens = NULL;
            if (strchr(token, '*')) {
                expanded_tokens = expand_wildcards(token);
            } else {
                expanded_tokens = malloc(sizeof(char *) * 2);
                if (!expanded_tokens) {
                    perror("malloc");
                    free_command(cmd);
                    return NULL;
                }
                expanded_tokens[0] = safe_strdup(token);
                expanded_tokens[1] = NULL;
            }

            if (expanded_tokens) {
                int i = 0;
                while (expanded_tokens[i]) {
                    current_cmd->argv[argc] = safe_strdup(expanded_tokens[i]);
                    argc++;
                    i++;
                    if (argc >= MAX_TOKENS - 1) {
                        fprintf(stderr, "Too many arguments\n");
                        free_command(cmd);
                        free_tokens(expanded_tokens);
                        return NULL;
                    }
                }
                free_tokens(expanded_tokens);
            } else {
                // Regular argument
                current_cmd->argv[argc] = safe_strdup(token);
                argc++;
                if (argc >= MAX_TOKENS - 1) {
                    fprintf(stderr, "Too many arguments\n");
                    free_command(cmd);
                    return NULL;
                }
            }
        }

        token_index++;
    }

    current_cmd->argc = argc;
    current_cmd->argv[argc] = NULL;

    // Set command name
    if (cmd->argv[0]) {
        cmd->cmd_name = safe_strdup(cmd->argv[0]);
        if (strcmp(cmd->cmd_name, "cd") == 0 ||
            strcmp(cmd->cmd_name, "pwd") == 0 ||
            strcmp(cmd->cmd_name, "which") == 0 ||
            strcmp(cmd->cmd_name, "exit") == 0) {
            cmd->is_builtin = 1;
            if (strcmp(cmd->cmd_name, "exit") == 0) {
                cmd->is_exit = 1;
            }
        }
    } else {
        fprintf(stderr, "No command provided\n");
        free_command(cmd);
        return NULL;
    }

    // **Debugging: Print the command structure**
    printf("Debug: Parsed Command Structure:\n");
    printf("  Command Name: '%s'\n", cmd->cmd_name);
    for (int i = 0; i < argc; i++) {
        printf("  Arg %d: '%s'\n", i, cmd->argv[i]);
    }

    return cmd;
}


void free_tokens(char **tokens) {
    if (tokens) {
        int i = 0;
        while (tokens[i]) {
            free(tokens[i]);
            i++;
        }
        free(tokens);
    }
}

void free_command(struct command *cmd) {
    if (cmd) {
        if (cmd->cmd_name) free(cmd->cmd_name);
        if (cmd->argv) {
            int i = 0;
            while (cmd->argv[i]) {
                free(cmd->argv[i]);
                i++;
            }
            free(cmd->argv);
        }
        if (cmd->input_file) free(cmd->input_file);
        if (cmd->output_file) free(cmd->output_file);
        if (cmd->next) {
            free_command(cmd->next);
        }
        free(cmd);
    }
}
