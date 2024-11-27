#include "mysh.h"

/**
 * Dynamic array implementation for managing arguments.
 */

// Initializes a dynamic array
void init_arraylist(arraylist_t *list) {
    list->capacity = INITIAL_CAPACITY;
    list->size = 0;
    list->items = malloc(sizeof(char *) * list->capacity);
}

// Adds an item to the dynamic array
void add_to_arraylist(arraylist_t *list, char *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(char *) * list->capacity);
    }
    list->items[list->size++] = strdup(item);
}

// Frees the memory used by the dynamic array
void free_arraylist(arraylist_t *list) {
    for (size_t i = 0; i < list->size; i++) {
        free(list->items[i]);
    }
    free(list->items);
}

