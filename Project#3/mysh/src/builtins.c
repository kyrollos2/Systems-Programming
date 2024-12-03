// builtins.c

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "builtins.h"
#include "utils.h"

#include <linux/limits.h>

int builtin_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: Missing argument\n");
        return 1;
    }
    if (args[2] != NULL) {
        fprintf(stderr, "cd: Too many arguments\n");
        return 1;
    }
    if (chdir(args[1]) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int builtin_pwd(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "pwd: Too many arguments\n");
        return 1;
    }
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("pwd");
        return 1;
    }
}

int builtin_which(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "which: Missing argument\n");
        return 1;
    }
    if (args[2] != NULL) {
        fprintf(stderr, "which: Too many arguments\n");
        return 1;
    }
    // Check if it's a built-in command
    if (strcmp(args[1], "cd") == 0 || strcmp(args[1], "pwd") == 0 ||
        strcmp(args[1], "which") == 0 || strcmp(args[1], "exit") == 0) {
        printf("%s: shell built-in command\n", args[1]);
        return 0;
    }
    // Search in predefined paths
    const char *paths[] = { "/usr/local/bin", "/usr/bin", "/bin", NULL };
    for (int i = 0; paths[i] != NULL; i++) {
        size_t path_length = strlen(paths[i]) + strlen(args[1]) + 2; // +2 for '/' and null terminator
        char *full_path = malloc(path_length);
        if (!full_path) {
            perror("malloc");
            return 1;
        }
        snprintf(full_path, path_length, "%s/%s", paths[i], args[1]);
        if (access(full_path, X_OK) == 0) {
            printf("%s\n", full_path);
            free(full_path);
            return 0;
        }
        free(full_path);
    }
    // Command not found
    fprintf(stderr, "%s: Command not found\n", args[1]);
    return 1;
}
