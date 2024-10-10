// memgrind.c
/*This will be used for stress testing because 
this monstrosity deserves to sweat after  what it did to me
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdalign.h>
#include <time.h>      // Include this header for time()
#include "mymalloc.h"


#define RUNS 50  // Number of times each workload will be executed for timing purposes

/*
 * Function: workload1
 * -------------------
 * Workload 1 allocates and immediately frees 120 blocks of 1 byte each.
 *
 * Steps:
 * 1. Loop 120 times to perform allocation and deallocation.
 * 2. In each iteration:
 *    a. Use malloc(1) to allocate 1 byte of memory.
 *    b. Immediately free the allocated memory using free(ptr).
 *
 * Purpose:
 * - Tests the allocator's ability to handle rapid allocation and deallocation.
 * - Checks for any overhead or memory leaks during quick succession of operations.
 */
void workload1() {
    for (int i = 0; i < 120; i++) {
        void *ptr = malloc(1);
        free(ptr);
    }
}

/*
 * Function: workload2
 * -------------------
 * Workload 2 allocates 120 blocks of 1 byte each, then frees them all.
 *
 * Steps:
 * 1. Declare an array of pointers to hold allocated memory blocks.
 * 2. Loop 120 times to allocate memory:
 *    a. Use malloc(1) to allocate 1 byte of memory.
 *    b. Store the pointer in the array.
 * 3. Loop 120 times to free the allocated memory:
 *    a. Use free(ptrs[i]) to free each allocated block.
 *
 * Purpose:
 * - Tests the allocator's ability to handle multiple allocations without immediate deallocation.
 * - Checks if the allocator properly tracks multiple allocations before freeing.
 */
void workload2() {
    void *ptrs[120];
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    }
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }
}

/*
 * Function: workload3
 * -------------------
 * Workload 3 randomly allocates and frees memory until all blocks are allocated and freed.
 *
 * Steps:
 * 1. Declare an array of pointers to hold allocated memory blocks.
 * 2. Initialize alloc_count and free_count to track allocations and deallocations.
 * 3. While alloc_count < 120:
 *    a. Randomly decide to allocate or free (50% chance each).
 *    b. If allocating and alloc_count < 120:
 *       - Allocate 1 byte using malloc(1) and store the pointer.
 *       - Increment alloc_count.
 *    c. Else if freeing and free_count < alloc_count:
 *       - Free the next allocated block.
 *       - Increment free_count.
 * 4. After allocation loop, free any remaining allocated memory.
 *
 * Purpose:
 * - Simulates unpredictable real-world memory usage patterns.
 * - Tests the allocator's robustness in handling random allocation and deallocation.
 */
void workload3() {
    void *ptrs[120];
    int alloc_count = 0;
    int free_count = 0;

    while (alloc_count < 120) {
        int action = rand() % 2;  // Randomly choose between 0 (allocate) and 1 (free)
        if (action == 0 && alloc_count < 120) {
            ptrs[alloc_count++] = malloc(1); // Allocate memory
        } else if (free_count < alloc_count) {
            free(ptrs[free_count++]); // Free memory
        }
    }

    // Free any remaining allocated memory
    while (free_count < alloc_count) {
        free(ptrs[free_count++]);
    }
}

/*
 * Function: workload4
 * -------------------
 * Workload 4 allocates and frees memory in a sequential manner.
 *
 * Steps:
 * 1. Declare an array of pointers to hold allocated memory blocks.
 * 2. Loop 60 times to allocate memory:
 *    a. Use malloc(20) to allocate 20 bytes (larger allocations).
 *    b. Store the pointer in the array.
 * 3. Loop from 59 down to 0 to free memory in reverse order:
 *    a. Use free(ptrs[i]) to free each allocated block.
 *
 * Purpose:
 * - Tests the allocator's ability to handle larger allocations.
 * - Checks how the allocator handles reverse order deallocation, simulating stack-like behavior.
 */
void workload4() {
    void *ptrs[60];
    for (int i = 0; i < 60; i++) {
        ptrs[i] = malloc(20); // Larger allocations
    }
    for (int i = 59; i >= 0; i--) {
        free(ptrs[i]); // Reverse order free
    }
}

/*
 * Function: workload5
 * -------------------
 * Workload 5 allocates memory in various sizes and then frees them randomly.
 *
 * Steps:
 * 1. Declare an array of pointers to hold allocated memory blocks.
 * 2. Loop 50 times to allocate memory:
 *    a. Calculate size as multiples of 15 (15, 30, 45, 60, 75 bytes).
 *    b. Use malloc(size) to allocate memory.
 *    c. Store the pointer in the array.
 * 3. Shuffle the array of pointers to randomize the order.
 * 4. Loop 50 times to free the allocated memory in random order.
 *
 * Purpose:
 * - Tests the allocator's handling of variable allocation sizes.
 * - Checks how the allocator manages fragmentation and random freeing order.
 */
void workload5() {
    void *ptrs[50];
    // Allocation phase
    for (int i = 0; i < 50; i++) {
        size_t size = (i % 5 + 1) * 15; // Allocations of 15, 30, 45, 60, and 75 bytes
        ptrs[i] = malloc(size);
    }
    // Shuffle pointers to randomize freeing order
    for (int i = 0; i < 50; i++) {
        int j = rand() % 50;
        void *temp = ptrs[i];
        ptrs[i] = ptrs[j];
        ptrs[j] = temp;
    }
    // Free phase
    for (int i = 0; i < 50; i++) {
        free(ptrs[i]);
    }
}

/*
 * Function: workload6
 * -------------------
 * Workload 6 is a stress test with random allocation sizes and random freeing.
 *
 * Steps:
 * 1. Declare an array of pointers to hold allocated memory blocks.
 * 2. Initialize alloc_count and free_count to track allocations and deallocations.
 * 3. While alloc_count < 200:
 *    a. Randomly decide to allocate or free (50% chance each).
 *    b. If allocating and alloc_count < 200:
 *       - Generate a random size between 1 and 128 bytes.
 *       - Allocate memory using malloc(size) and store the pointer.
 *       - Increment alloc_count.
 *    c. Else if freeing and free_count < alloc_count:
 *       - Free the next allocated block.
 *       - Increment free_count.
 * 4. After allocation loop, free any remaining allocated memory.
 *
 * Purpose:
 * - Tests the allocator under heavy stress with unpredictable allocation patterns.
 * - Checks for robustness, memory leaks, and fragmentation handling.
 */
void workload6() {
    void *ptrs[200];
    int alloc_count = 0;
    int free_count = 0;

    while (alloc_count < 200) {
        int action = rand() % 2;  // Randomly choose between 0 (allocate) and 1 (free)
        if (action == 0 && alloc_count < 200) {
            size_t size = rand() % 128 + 1; // Random size between 1 and 128 bytes
            ptrs[alloc_count++] = malloc(size);
        } else if (free_count < alloc_count) {
            free(ptrs[free_count++]);
        }
    }

    // Free any remaining allocated memory
    while (free_count < alloc_count) {
        free(ptrs[free_count++]);
    }
}

/*
 * Function: workload7
 * -------------------
 * Workload 7 attempts to allocate the maximum possible size and then frees it.
 *
 * Steps:
 * 1. Calculate the maximum allocatable size as total heap size minus header size.
 * 2. Use malloc(max_size) to attempt allocation.
 * 3. If allocation succeeds, free the allocated memory.
 *
 * Purpose:
 * - Tests the allocator's ability to handle large allocations.
 * - Checks for proper handling of maximum allocation requests.
 */
void workload7() {
    // Attempt to allocate a large block
    size_t max_size = 4000; // Adjust this value as appropriate (close to heap size)
    void *ptr = malloc(max_size); // Attempt to allocate large block
    if (ptr) {
        free(ptr);
    } else {
        printf("Allocation of large block of size %zu failed.\n", max_size);
    }
}

/*
 * Function: workload8
 * -------------------
 * Workload 8 tests zero-size allocation and freeing.
 *
 * Steps:
 * 1. Use malloc(0) to attempt allocation of zero bytes.
 * 2. If malloc returns a non-NULL pointer, free it.
 *
 * Purpose:
 * - Checks if the allocator handles requests for zero bytes gracefully.
 * - Ensures that such requests do not cause errors or unexpected behavior.
 */
void workload8() {
    void *ptr = malloc(0); // Should handle gracefully
    if (ptr) {
        free(ptr); // Free even if malloc(0) returns a valid pointer
    }
}

/*
 * Function: main
 * --------------
 * Runs each workload multiple times and measures the average execution time.
 *
 * Steps:
 * 1. Seed the random number generator using srand.
 * 2. For each workload:
 *    a. Use gettimeofday to record the start time.
 *    b. Loop RUNS times to execute the workload.
 *    c. Use gettimeofday to record the end time.
 *    d. Calculate the average execution time in microseconds.
 *    e. Print the execution time for the workload.
 *
 * Purpose:
 * - Measures the performance of the allocator under different workloads.
 * - Provides insights into execution times for various memory usage patterns.
 */
int main() {
    struct timeval start, end;

    srand((unsigned int)time(NULL)); // Seed the random number generator

    // Measure time for workload 1
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload1();
    }
    gettimeofday(&end, NULL);
    printf("Workload 1: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 2
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload2();
    }
    gettimeofday(&end, NULL);
    printf("Workload 2: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 3
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload3();
    }
    gettimeofday(&end, NULL);
    printf("Workload 3: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 4
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload4();
    }
    gettimeofday(&end, NULL);
    printf("Workload 4: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 5
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload5();
    }
    gettimeofday(&end, NULL);
    printf("Workload 5: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 6
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload6();
    }
    gettimeofday(&end, NULL);
    printf("Workload 6: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 7
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload7();
    }
    gettimeofday(&end, NULL);
    printf("Workload 7: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 8
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload8();
    }
    gettimeofday(&end, NULL);
    printf("Workload 8: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec)) / RUNS);

    return 0; // Return success
}
