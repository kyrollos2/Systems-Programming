// wildcard.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <errno.h>

#include "wildcard.h"
#include "utils.h"

#define MAX_MATCHES 1024

void free_matches(char **matches);

char **expand_wildcards(const char *pattern) {
    char *directory = NULL;
    char *pattern_copy = NULL;
    char *base_pattern = NULL;
    DIR *dir = NULL;
    struct dirent *entry;
    char **matches = NULL;
    int match_count = 0;

    matches = malloc(sizeof(char *) * MAX_MATCHES);
    if (!matches) {
        perror("malloc");
        return NULL;
    }

    // Initialize matches array
    for (int i = 0; i < MAX_MATCHES; i++) {
        matches[i] = NULL;
    }

    // Duplicate the pattern to work with
    pattern_copy = strdup(pattern);
    if (!pattern_copy) {
        perror("strdup");
        free(matches);
        return NULL;
    }

    // Find the last slash to separate directory and pattern
    char *last_slash = strrchr(pattern_copy, '/');
    if (pattern_copy[0] == '/') {
        // Absolute path
        directory = "/";
        base_pattern = pattern_copy + 1;
    } else if (strncmp(pattern_copy, "./", 2) == 0) {
        // Current directory
        directory = ".";
        base_pattern = pattern_copy + 2;
    } else if (last_slash) {
        // Relative directory
        *last_slash = '\0';
        directory = pattern_copy;
        base_pattern = last_slash + 1;
    } else {
        // No directory path
        directory = ".";
        base_pattern = pattern_copy;
    }

    // Open the directory
    dir = opendir(directory);
    if (!dir) {
        // If directory cannot be opened, return the original pattern
        matches[0] = strdup(pattern);
        matches[1] = NULL;
        free(pattern_copy);
        return matches;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files unless pattern starts with '.'
        if (entry->d_name[0] == '.' && base_pattern[0] != '.') {
            continue;
        }

        // Use fnmatch to match the pattern
        if (fnmatch(base_pattern, entry->d_name, 0) == 0) {
            // Build the full path if necessary
            if (strcmp(directory, ".") != 0) {
                size_t path_length = strlen(directory) + strlen(entry->d_name) + 2;
                char *full_path = malloc(path_length);
                if (!full_path) {
                    perror("malloc");
                    // Free allocated memory before returning
                    free_matches(matches);
                    free(pattern_copy);
                    closedir(dir);
                    return NULL;
                }
                snprintf(full_path, path_length, "%s/%s", directory, entry->d_name);
                matches[match_count] = full_path; // Assign the allocated path
            } else {
                matches[match_count] = strdup(entry->d_name);
            }

            match_count++;
            if (match_count >= MAX_MATCHES - 1) {
                fprintf(stderr, "Too many matches for pattern: %s\n", pattern);
                break;
            }
        }
    }

    closedir(dir);
    free(pattern_copy);

    if (match_count == 0) {
        // No matches found, return the original pattern
        matches[0] = strdup(pattern);
        matches[1] = NULL;
    } else {
        matches[match_count] = NULL; // Null-terminate the array
    }

    return matches;
}

void free_matches(char **matches) {
    if (matches) {
        for (int i = 0; matches[i] != NULL; i++) {
            free(matches[i]);
        }
        free(matches);
    }
}
