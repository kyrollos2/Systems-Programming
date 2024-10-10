// mymalloc.c
/*Linear implementation of mymalloc
I wanted to do a  graph based implementation
where we could allocated neighbor hoods of nodes based
on the allocation request size but i am beholden to your
autograder and the time sink my drinking problem causes*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
#define MIN_BLOCK_SIZE (8)  // Minimum size for a usable block (excluding header)

typedef struct chunk_header {
    size_t is_free : 1;
    size_t size    : 63;  // Assuming size_t is 64 bits
} chunk_header;
//forward declarations of methods
void initialize_heap();
void *mymalloc(size_t size, char *file, int line);
void myfree(void *ptr, char *file, int line);
void coalesce(chunk_header *chunk);
void leak_detector();

static union {
    char bytes[MEMLENGTH];
    long long align;  // ensure heap is aligned
} heap;

static chunk_header *base = NULL;
static bool initialized = false;

/*
 * Function: initialize_heap
 * -------------------------
 * Initializes the simulated heap memory if it hasn't been initialized yet.
 *
 * Steps:
 * 1. Check if the heap has already been initialized using the 'initialized' flag.
 * 2. If not initialized:
 *    a. Set 'base' to point to the start of the heap memory.
 *    b. Set the size of the base chunk to the total heap size minus the size of the header.
 *    c. Mark the base chunk as free.
 *    d. Set 'initialized' to true to prevent reinitialization.
 *    e. Register the 'leak_detector' function to run at program exit using 'atexit'.
 */

void initialize_heap() {
    if (!initialized) {
        base = (chunk_header*)heap.bytes;
        base->size = MEMLENGTH - sizeof(chunk_header);
        base->is_free = 1;
        initialized = true;
        atexit(leak_detector);
    }
}
/*
 * Function: mymalloc
 * ------------------
 * Allocates a block of memory of the given size.
 *
 * Steps:
 * 1. Initialize the heap if it hasn't been initialized yet.
 * 2. Return NULL if the requested size is 0.
 * 3. Align the requested size to 8 bytes for proper memory alignment.
 * 4. Start from the base of the heap and traverse each chunk sequentially.
 * 5. For each chunk:
 *    a. Check if the chunk is free and large enough to accommodate the requested size.
 *    b. If it is:
 *       i. Determine if the chunk can be split by checking if the remaining space after allocation
 *          is enough for a new chunk (including header and minimum block size).
 *       ii. If it can be split:
 *           - Create a new chunk header in the remaining space.
 *           - Set the size and mark it as free.
 *           - Adjust the current chunk's size to the requested size.
 *       iii. If it cannot be split:
 *           - Allocate the entire chunk without splitting.
 *       iv. Mark the current chunk as used.
 *       v. Return a pointer to the user data area (just after the chunk header).
 * 6. If no suitable chunk is found, print an error message and return NULL.
 *
 * Parameters:
 *   size - The size of memory to allocate.
 *   file - The source file from which mymalloc is called (for error reporting).
 *   line - The line number in the source file (for error reporting).
 *
 * Returns:
 *   A pointer to the allocated memory block, or NULL if allocation fails.
 */
void *mymalloc(size_t size, char *file, int line) {
    if (!initialized) {
        initialize_heap();
    }

    if (size == 0) {
        return NULL;
    }

    // Align size to 8 bytes
    size = (size + 7) & ~((size_t)7);

    chunk_header *current = base;

    while ((char*)current < heap.bytes + MEMLENGTH) {
        if (current->is_free && current->size >= size) {
            // Check if we can split the chunk
            if (current->size >= size + sizeof(chunk_header) + MIN_BLOCK_SIZE) {
                // Split the chunk
                size_t remaining_size = current->size - size - sizeof(chunk_header);
                chunk_header *next_chunk = (chunk_header*)((char*)current + sizeof(chunk_header) + size);
                next_chunk->size = remaining_size;
                next_chunk->is_free = 1;

                current->size = size;
                current->is_free = 0;
            } else {
                // Do not split; allocate the entire chunk
                current->is_free = 0;
            }
            // Return a pointer to the user data area
            return (char*)current + sizeof(chunk_header);
        }
        // Move to the next chunk
        current = (chunk_header*)((char*)current + sizeof(chunk_header) + current->size);
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}
/*
 * Function: myfree
 * ----------------
 * Frees a previously allocated block of memory.
 *
 * Steps:
 * 1. Check if the pointer is NULL; if so, do nothing.
 * 2. Retrieve the chunk header corresponding to the memory block by subtracting the size of the header from the pointer.
 * 3. Check if the chunk is already free; if so, report a double free error and exit.
 * 4. Mark the chunk as free.
 * 5. Call 'coalesce' to merge the freed chunk with adjacent free chunks if possible.
 *
 * Parameters:
 *   ptr  - The pointer to the memory block to free.
 *   file - The source file from which myfree is called (for error reporting).
 *   line - The line number in the source file (for error reporting).
 */
void myfree(void *ptr, char *file, int line) {
    if (!ptr) {
        return;
    }

    // Get the chunk header
    chunk_header *chunk = (chunk_header*)((char*)ptr - sizeof(chunk_header));

    if (chunk->is_free) {
        fprintf(stderr, "free: Double free detected (%s:%d)\n", file, line);
        exit(EXIT_FAILURE);
    }

    chunk->is_free = 1;
    coalesce(chunk);
}
/*
 * Function: coalesce
 * ------------------
 * Attempts to merge the given free chunk with adjacent free chunks to reduce fragmentation.
 *
 * Steps:
 * 1. Coalesce with the next chunk:
 *    a. Calculate the address of the next chunk by adding the size of the current chunk and its header.
 *    b. Check if the next chunk is within the heap bounds and is free.
 *    c. If so, merge the next chunk into the current chunk by adding its size (including header).
 * 2. Coalesce with the previous chunk:
 *    a. Check if the current chunk is not the base chunk.
 *    b. If not, start from the base and traverse the heap to find the previous chunk.
 *    c. While traversing, move to the next chunk until reaching the one just before the current chunk.
 *    d. Check if the previous chunk is free and adjacent to the current chunk.
 *    e. If so, merge the current chunk into the previous chunk by adding its size (including header).
 *
 * Parameters:
 *   chunk - The chunk to coalesce with adjacent free chunks.
 */
void coalesce(chunk_header *chunk) {
    // Coalesce with next chunk if it's free
    chunk_header *next_chunk = (chunk_header*)((char*)chunk + sizeof(chunk_header) + chunk->size);
    if ((char*)next_chunk < heap.bytes + MEMLENGTH && next_chunk->is_free) {
        chunk->size += sizeof(chunk_header) + next_chunk->size;
    }

    // Coalesce with previous chunk if it's free
    if (chunk != base) {
        chunk_header *prev_chunk = base;
        while ((char*)prev_chunk + sizeof(chunk_header) + prev_chunk->size < (char*)chunk) {
            prev_chunk = (chunk_header*)((char*)prev_chunk + sizeof(chunk_header) + prev_chunk->size);
        }
        if (prev_chunk->is_free && (char*)prev_chunk + sizeof(chunk_header) + prev_chunk->size == (char*)chunk) {
            prev_chunk->size += sizeof(chunk_header) + chunk->size;
            chunk = prev_chunk;
        }
    }
}
/*
 * Function: leak_detector
 * -----------------------
 * Scans the heap at program exit to detect any memory leaks.
 *
 * Steps:
 * 1. Initialize variables to track total leaked memory and the count of leaked objects.
 * 2. Start from the base of the heap and traverse each chunk sequentially.
 * 3. For each chunk:
 *    a. If the chunk is not free (allocated), add its size to the total leaked memory and increment the count.
 * 4. After traversal, check if any memory leaks were detected.
 * 5. If leaks are found, print a message reporting the total leaked bytes and the number of leaked objects.
 *
 * Note:
 * - This function is registered to run automatically at program exit using 'atexit' in 'initialize_heap'.
 */
void leak_detector() {
    size_t total_leaked = 0;
    int count = 0;
    chunk_header *current = base;
    while ((char*)current < heap.bytes + MEMLENGTH) {
        if (!current->is_free) {
            total_leaked += current->size;
            count++;
        }
        current = (chunk_header*)((char*)current + sizeof(chunk_header) + current->size);
    }
    if (total_leaked > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", total_leaked, count);
    }
}
