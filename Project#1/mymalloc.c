#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used; // Ensure 8-byte alignment
} heap;

typedef struct chunk_header {
    size_t size;
    bool is_free;
    struct chunk_header* next;
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
        atexit(leak_detector);
        initialized = true;
    }
}

void *mymalloc(size_t size, char *file, int line) {
    if (!initialized) {
        initialize_heap();
    }

    size = align8(size); // Align requested size to 8 bytes
    chunk_header *current = base;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            // Check if there's enough space to split the chunk
            if (current->size >= size + sizeof(chunk_header) + 8) {
                chunk_header *new_chunk = (chunk_header*)((char*)current + sizeof(chunk_header) + size);
                new_chunk->size = current->size - size - sizeof(chunk_header);
                new_chunk->is_free = true;
                new_chunk->next = current->next;
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
    chunk_header *current = base;

    // Check if the pointer is at the start of a chunk
    while (current != NULL && (void*)current != (void*)chunk) {
        current = current->next;
    }

    if (!current || !chunk->is_free) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    chunk->is_free = true;

    // Coalescing adjacent free blocks
    current = base;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += current->next->size + sizeof(chunk_header);
            current->next = current->next->next;
        }
        current = current->next;
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
