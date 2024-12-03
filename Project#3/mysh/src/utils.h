// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// Function declarations

/**
 * Trims leading and trailing whitespace from a string.
 * The function modifies the string in place and returns a pointer to the trimmed string.
 *
 * @param str The input string to trim.
 * @return A pointer to the trimmed string.
 */
char *trim_whitespace(char *str);

/**
 * Duplicates a string safely. Similar to strdup but checks for allocation failure.
 *
 * @param str The input string to duplicate.
 * @return A pointer to the duplicated string, or exits on failure.
 */
char *safe_strdup(const char *str);

/**
 * Allocates memory safely. Similar to malloc but checks for allocation failure.
 *
 * @param size The size in bytes to allocate.
 * @return A pointer to the allocated memory, or exits on failure.
 */
void *safe_malloc(size_t size);

/**
 * Prints an error message to stderr.
 *
 * @param message The error message to print.
 */
void print_error(const char *message);

#endif // UTILS_H
