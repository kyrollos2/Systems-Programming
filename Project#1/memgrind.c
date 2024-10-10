#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define RUNS 50  // Number of times each workload will be executed for timing purposes

// Allocates and immediately frees 120 blocks of memory.
void workload1() {
    for (int i = 0; i < 120; i++) {
        void *ptr = malloc(1);
        free(ptr);
    }
}

// Allocates 120 blocks of memory and then frees them all.
void workload2() {
    void *ptrs[120];
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    }
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }
}

// Randomly allocates and frees memory until all blocks are allocated and freed.
void workload3() {
    void *ptrs[120];
    int alloc_count = 0;
    int free_count = 0;

    while (alloc_count < 120) {
        int action = rand() % 2;
        if (action == 0 && alloc_count < 120) {
            ptrs[alloc_count++] = malloc(1); // Allocate memory
        } else if (free_count < alloc_count) {
            free(ptrs[free_count++]); // Free memory
        }
    }

    while (free_count < alloc_count) {
        free(ptrs[free_count++]); // Free remaining allocated memory
    }
}

// Allocates and frees memory in a sequential manner.
void workload4() {
    void *ptrs[60];
    for (int i = 0; i < 60; i++) {
        ptrs[i] = malloc(20); // Larger allocations
    }
    for (int i = 59; i >= 0; i--) {
        free(ptrs[i]); // Reverse order free
    }
}

// Allocates memory in various sizes and then frees them randomly.
void workload5() {
    void *ptrs[50];
    for (int i = 0; i < 50; i++) {
        size_t size = (i % 5 + 1) * 15; // Allocations of 15, 30, 45, 60, and 75 bytes
        ptrs[i] = malloc(size);
    }
    // Shuffle pointers before freeing
    for (int i = 0; i < 50; i++) {
        int j = rand() % 50;
        void *temp = ptrs[i];
        ptrs[i] = ptrs[j];
        ptrs[j] = temp;
    }
    for (int i = 0; i < 50; i++) {
        free(ptrs[i]);
    }
}

// Runs each workload and measures the execution time.
int main() {
    struct timeval start, end;

    // Measure time for workload 1
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload1();
    }
    gettimeofday(&end, NULL);
    printf("Workload 1: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 2
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload2();
    }
    gettimeofday(&end, NULL);
    printf("Workload 2: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 3
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload3();
    }
    gettimeofday(&end, NULL);
    printf("Workload 3: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 4
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload4();
    }
    gettimeofday(&end, NULL);
    printf("Workload 4: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / RUNS);

    // Measure time for workload 5
    gettimeofday(&start, NULL);
    for (int i = 0; i < RUNS; i++) {
        workload5();
    }
    gettimeofday(&end, NULL);
    printf("Workload 5: %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / RUNS);

    return 0; // return success
}
