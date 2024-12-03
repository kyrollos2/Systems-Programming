// utils.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

char *trim_whitespace(char *str) {
    if (!str) return NULL;

    char *end;

    // Trim leading whitespace
    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0') {
        // All whitespace
        return str;
    }

    // Trim trailing whitespace
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

char *safe_strdup(const char *str) {
    if (!str) return NULL;

    char *dup = strdup(str);
    if (!dup) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    return dup;
}

void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void print_error(const char *message) {
    if (message) {
        fprintf(stderr, "Error: %s\n", message);
    } else {
        fprintf(stderr, "An unknown error occurred.\n");
    }
}
