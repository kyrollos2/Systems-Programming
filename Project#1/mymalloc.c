#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used;  // Ensure 8-byte alignment
} heap;

typedef struct chunk_header {
    size_t size;
    bool is_free;
    struct chunk_header* next;
    struct chunk_header* prev;  // Added for backward coalescing
} chunk_header;

static chunk_header* base = NULL;
static bool initialized = false;

// Forward declarations
void initialize_heap();
void leak_detector();

// Utility function to align the size to 8 bytes
size_t align8(size_t size) {
    return (size + 7) & ~7;
}

void initialize_heap() {
    if (!initialized) {
        base = (chunk_header*)heap.bytes;
        base->size = MEMLENGTH - sizeof(chunk_header);
        base->is_free = true;
        base->next = NULL;
        base->prev = NULL;  // Initialize previous pointer
        atexit(leak_detector);
        initialized = true;
    }
}

void *mymalloc(size_t size, char *file, int line) {
    if (!initialized) {
        initialize_heap();
    }

    size = align8(size);
    chunk_header *current = base;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            // Enhanced Splitting Logic: Avoid excessive fragmentation
            if (current->size >= size + sizeof(chunk_header) + align8(1)) {
                chunk_header *new_chunk = (chunk_header*)((char*)current + sizeof(chunk_header) + size);
                new_chunk->size = current->size - size - sizeof(chunk_header);
                new_chunk->is_free = true;
                new_chunk->next = current->next;
                new_chunk->prev = current;  // Set the new chunk's previous pointer
                if (current->next) {
                    current->next->prev = new_chunk;
                }
                current->size = size;
                current->next = new_chunk;
            }
            current->is_free = false;
            return (char*)current + sizeof(chunk_header);
        }
        current = current->next;
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    if (!ptr) {
        return; // Freeing NULL is a no-op
    }

    chunk_header *chunk = (chunk_header*)((char*)ptr - sizeof(chunk_header));
    if (chunk->is_free) {
        fprintf(stderr, "free: Double free detected (%s:%d)\n", file, line);
        exit(2);
    }

    chunk->is_free = true;

    // Coalescing with previous and next blocks
    if (chunk->prev && chunk->prev->is_free) {
        chunk->prev->size += chunk->size + sizeof(chunk_header);
        chunk->prev->next = chunk->next;
        if (chunk->next) {
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    if (chunk->next && chunk->next->is_free) {
        chunk->size += chunk->next->size + sizeof(chunk_header);
        chunk->next = chunk->next->next;
        if (chunk->next) {
            chunk->next->prev = chunk;
        }
    }
}

void leak_detector() {
    size_t total_leaked = 0;
    int count = 0;
    chunk_header *current = base;

    while (current != NULL) {
        if (!current->is_free) {
            total_leaked += current->size;
            count++;
        }
        current = current->next;
    }

    if (total_leaked > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", total_leaked, count);
    }
}
