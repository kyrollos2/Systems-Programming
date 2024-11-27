#include "mysh.h"

/**
 * Implements built-in commands for the shell.
 */

// Handles the "cd" command
void change_directory(char **args) {
    if (!args[1]) {
        fprintf(stderr, "cd: Missing argument\n");
    } else if (chdir(args[1]) != 0) {
        perror("cd failed");
    }
}

// Handles the "pwd" command
void print_working_directory() {
    char current_dir[PATH_MAX];
    if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
        printf("%s\n", current_dir);
    } else {
        perror("Failed to get current directory");
    }
}

// Handles the "which" command
void find_program_location(char **args) {
    if (!args[1]) {
        fprintf(stderr, "which: Missing argument\n");
        return;
    }

    char *search_paths[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};
    for (int i = 0; search_paths[i]; i++) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", search_paths[i], args[1]);

        if (access(full_path, X_OK) == 0) {
            printf("%s\n", full_path);
            return;
        }
    }

    fprintf(stderr, "which: Command not found: %s\n", args[1]);
}

// Handles the "exit" command
void terminate_shell(char **args) {
    printf("Exiting shell...\n");
    if (args[1]) {
        printf("Arguments provided: %s\n", args[1]);
    }
    exit(0);
}

