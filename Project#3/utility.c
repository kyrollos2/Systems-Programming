#include "mysh.h"

/**
 * Contains utility functions for shell operations.
 */

// Prints the welcome message
void show_welcome_message() {
    printf("Welcome to mysh, your custom shell!\n");
}

// Prints the exit message
void show_exit_message() {
    printf("Thank you for using mysh. Goodbye!\n");
}

// Prints the shell prompt
void display_shell_prompt() {
    printf("mysh> ");
}

// Prints error messages
void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
}

// Checks if a command is a built-in
bool is_builtin_command(const char *command) {
    const char *builtins[] = {"cd", "pwd", "which", "exit", NULL};
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(command, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Searches for an executable in predefined directories
char *locate_executable(const char *program_name) {
    static char executable_path[PATH_MAX];
    char *search_paths[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};

    for (int i = 0; search_paths[i]; i++) {
        snprintf(executable_path, sizeof(executable_path), "%s/%s", search_paths[i], program_name);

        if (access(executable_path, X_OK) == 0) {
            return executable_path;
        }
    }

    return NULL;
}

