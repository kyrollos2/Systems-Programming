#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define RUNS 50  // Number of times each workload will be executed for timing purposes



//Allocates and immediately frees 120 blocks of memory.
void workload1() {
    for (int i = 0; i < 120; i++) {
        void *ptr = malloc(1);
        free(ptr);
    }
}


//Allocates 120 blocks of memory and then frees them all.
void workload2() {
    void *ptrs[120];
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    }
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }
}



//Randomly allocates and frees memory until all blocks are allocated and freed.
void workload3() {
    void *ptrs[120];
    int alloc_count = 0;
    int free_count = 0;

    
    while (alloc_count < 120) {
        int action = random() % 2;
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

//Runs each workload and measures the execution time.
// Uses gettimeofday() to get the start and end times, and calculates the average time taken.

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

    return 0; //return success


}


