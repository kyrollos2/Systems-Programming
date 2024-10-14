// mymalloc_small_batch_tests.c
/* The purpose of this vs memgrind is to do
   small scale testing of edge cases */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <time.h>      // Include this header for time()
#include "mymalloc.h"

/*
 * Function: test_basic_allocation
 * --------------------------------
 * Tests basic memory allocation and deallocation.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Use malloc to allocate memory for an integer.
 * 3. Check if the allocation was successful.
 *    a. If successful:
 *       - Assign the value 42 to the allocated integer.
 *       - Print the assigned value.
 *       - Free the allocated memory using free().
 *       - Print a message indicating the memory was freed.
 *    b. If allocation failed:
 *       - Print a message indicating allocation failure.
 *
 * Purpose:
 * - Verifies that the allocator can successfully allocate and free memory.
 * - Checks for correctness in storing and retrieving data.
 */
void test_basic_allocation() {
    printf("Test Basic Allocation and Freeing:\n");
    int *p = malloc(sizeof(int));
    if (p) {
        *p = 42;
        printf("    Allocated int and set to %d\n", *p);
        free(p);
        printf("    Freed int\n");
    } else {
        printf("    Allocation failed\n");
    }
}

/*
 * Function: test_exhaustive_allocation
 * -------------------------------------
 * Tests memory allocation until the heap is exhausted.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Declare an array of pointers to hold allocated memory blocks.
 * 3. Initialize a counter 'i' to 0.
 * 4. Loop to allocate memory blocks:
 *    a. Use malloc(10) to allocate 10 bytes.
 *    b. Store the pointer in ptrs[i].
 *    c. Continue looping until allocation fails or 1000 allocations are made.
 * 5. Print the number of blocks successfully allocated.
 * 6. Loop in reverse to free all allocated memory blocks.
 *
 * Purpose:
 * - Tests the allocator's behavior when the heap becomes full.
 * - Checks if the allocator gracefully handles memory exhaustion.
 */
void test_exhaustive_allocation() {
    printf("Test Exhaustive Allocation:\n");
    void *ptrs[1000];
    int i = 0;
    // Attempt to allocate 10 bytes repeatedly until failure or limit reached
    for (; i < 1000 && (ptrs[i] = malloc(10)); i++);
    printf("    Allocated %d blocks of 10 bytes until exhaustion\n", i);
    // Free all allocated memory blocks
    while (--i >= 0) free(ptrs[i]);
}

/*
 * Function: test_free_coalescing
 * ------------------------------
 * Tests whether the allocator correctly coalesces adjacent free blocks.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Allocate three memory blocks 'a', 'b', and 'c', each of 100 bytes.
 * 3. Print a message indicating the allocations.
 * 4. Free block 'a' (first block) and print a message.
 * 5. Free block 'c' (third block) and print a message.
 * 6. Free block 'b' (second block) and print a message.
 *    - This should trigger coalescing of all three blocks into one large free block.
 *
 * Purpose:
 * - Verifies that the allocator merges adjacent free blocks to reduce fragmentation.
 * - Ensures that coalescing occurs when appropriate.
 */
void test_free_coalescing() {
    printf("Test Freeing and Coalescing:\n");
    void *a = malloc(100);
    void *b = malloc(100);
    void *c = malloc(100);
    printf("    Allocated three blocks\n");
    free(a);
    printf("    Freed first block\n");
    free(c);
    printf("    Freed third block\n");
    free(b); // Should trigger coalescing
    printf("    Freed second block, coalescing should occur\n");
}

/*
 * Function: test_double_free
 * --------------------------
 * Tests the allocator's handling of a double free error.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Allocate memory for an integer.
 * 3. Check if the allocation was successful.
 *    a. If successful:
 *       - Free the allocated memory.
 *       - Print a message indicating the first free.
 *       - Uncomment the second free to test double free detection (optional).
 *       - Print a message indicating the attempted second free.
 *    b. If allocation failed:
 *       - Print a message indicating allocation failure.
 *
 * Note:
 * - The second free is commented out because performing a double free may cause the program to exit or crash.
 * - Uncomment the second free to test the allocator's error handling for double frees.
 *
 * Purpose:
 * - Ensures that the allocator detects and handles double free errors.
 * - Prevents undefined behavior or memory corruption due to double frees.
 */
void test_double_free() {
    printf("Test Double Free:\n");
    int *p = malloc(sizeof(int));
    if (p) {
        free(p);
        printf("    Freed once\n");
        // Uncomment the next line to test double free (will exit program)
        // free(p);
        // printf("    Attempted to free again\n");
    } else {
        printf("    Allocation failed\n");
    }
}

/*
 * Function: test_zero_size_allocation
 * ------------------------------------
 * Tests the allocator's handling of zero-size memory allocation requests.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Attempt to allocate zero bytes using malloc(0).
 * 3. Check if the returned pointer is NULL.
 *    a. If NULL:
 *       - Print a message indicating that NULL was returned as expected.
 *    b. If not NULL:
 *       - Print a message indicating that a non-NULL pointer was returned.
 *       - Free the allocated memory (if any).
 *
 * Purpose:
 * - Verifies that the allocator correctly handles zero-size allocation requests.
 * - Ensures that such requests do not cause errors or unexpected behavior.
 */
void test_zero_size_allocation() {
    printf("Test Zero-Size Allocation:\n");
    void *p = malloc(0);
    if (p == NULL) {
        printf("    Allocation of zero bytes returned NULL as expected\n");
    } else {
        printf("    Allocation of zero bytes did not return NULL\n");
        free(p);
    }
}

/*
 * Function: test_free_null_pointer
 * --------------------------------
 * Tests the allocator's handling of freeing a NULL pointer.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Call free(NULL) to attempt to free a NULL pointer.
 * 3. Print a message indicating that free(NULL) was executed without crashing.
 *
 * Purpose:
 * - Ensures that freeing a NULL pointer does not cause any issues.
 * - Confirms that the allocator handles this case gracefully.
 */
void test_free_null_pointer() {
    printf("Test Freeing NULL Pointer:\n");
    free(NULL);
    printf("    free(NULL) executed without crashing\n");
}

/*
 * Function: test_free_invalid_pointer
 * ------------------------------------
 * Tests the allocator's handling of freeing an invalid pointer.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Declare an integer variable 'x' on the stack.
 * 3. Set a pointer 'p' to the address of 'x'.
 * 4. Uncomment the free(p) line to attempt to free a stack variable.
 * 5. Print a message indicating the attempted invalid free.
 *
 * Note:
 * - Attempting to free an invalid pointer may cause undefined behavior or crash the program.
 * - The free(p) line is commented out to prevent accidental execution.
 *
 * Purpose:
 * - Tests the allocator's ability to detect and handle invalid free operations.
 * - Ensures robustness against erroneous use of free.
 */
void test_free_invalid_pointer() {
    printf("Test Freeing Invalid Pointer:\n");
    int x;
    int *p = &x;  // Stack variable
    (void)p;      // Suppress unused variable warning
    // Uncomment the next line to test invalid free (will cause undefined behavior)
    // free(p);
    // printf("    Attempted to free an invalid pointer\n");
}

/*
 * Function: test_alignment
 * ------------------------
 * Tests whether the allocator returns properly aligned memory for specific data types.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Allocate memory for a double using malloc(sizeof(double)).
 * 3. Check if the allocation was successful.
 *    a. If successful:
 *       - Check if the pointer 'p' is properly aligned for a double.
 *         - Use the alignof operator and uintptr_t for alignment checking.
 *       - Print a message indicating whether the memory is properly aligned.
 *       - Free the allocated memory.
 *    b. If allocation failed:
 *       - Print a message indicating allocation failure.
 *
 * Purpose:
 * - Ensures that the allocator provides memory with the correct alignment requirements.
 * - Prevents undefined behavior caused by misaligned memory accesses.
 */
void test_alignment() {
    printf("Test Memory Alignment:\n");
    double *p = malloc(sizeof(double));
    if (p) {
        if (((uintptr_t)p % alignof(double)) == 0) {
            printf("    Memory is properly aligned for double\n");
        } else {
            printf("    Memory is not properly aligned for double\n");
        }
        free(p);
    } else {
        printf("    Allocation failed\n");
    }
}

/*
 * Function: test_large_allocation
 * -------------------------------
 * Tests the allocator's handling of large memory allocation requests.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Calculate a large allocation size close to the total heap size.
 *    - Since 'chunk_header' is internal to the allocator and not accessible here,
 *      we use a fixed value less than the heap size (e.g., 4000 bytes).
 * 3. Attempt to allocate memory of size 'large_size' using malloc(large_size).
 * 4. Check if the allocation was successful.
 *    a. If successful:
 *       - Print a message indicating successful allocation.
 *       - Free the allocated memory.
 *    b. If allocation failed:
 *       - Print a message indicating allocation failure.
 *
 * Purpose:
 * - Verifies that the allocator can handle large allocation requests.
 * - Tests the allocator's response when requesting a significant portion of the heap.
 */
void test_large_allocation() {
    printf("Test Large Allocation:\n");
    size_t large_size = 4000; // Adjust this value as appropriate (close to heap size)
    void *p = malloc(large_size);
    if (p) {
        printf("    Successfully allocated a large block of size %zu bytes\n", large_size);
        free(p);
    } else {
        printf("    Failed to allocate a large block of size %zu bytes\n", large_size);
    }
}

/*
 * Function: test_stress_random_sizes
 * -----------------------------------
 * Performs a stress test by allocating and freeing memory blocks of random sizes.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Declare an array of pointers to hold allocated memory blocks.
 * 3. Initialize 'alloc_count' and 'free_count' to track allocations and frees.
 * 4. While 'alloc_count' is less than 1000:
 *    a. Generate a random size between 1 and 128 bytes.
 *    b. Attempt to allocate memory of the random size.
 *    c. If allocation is successful:
 *       - Store the pointer in ptrs[alloc_count].
 *       - Increment 'alloc_count'.
 *    d. If allocation fails:
 *       - Break the loop.
 * 5. Print the number of blocks successfully allocated.
 * 6. Shuffle the array of pointers to randomize the freeing order.
 * 7. While 'free_count' is less than 'alloc_count':
 *    a. Free the memory block at ptrs[free_count].
 *    b. Increment 'free_count'.
 * 8. Print a message indicating that all allocated blocks have been freed.
 *
 * Purpose:
 * - Tests the allocator's robustness under stress with random allocation sizes.
 * - Checks for memory leaks, fragmentation, and proper memory management.
 */
void test_stress_random_sizes() {
    printf("Test Stress with Random Sizes:\n");
    void *ptrs[1000];
    int alloc_count = 0;
    int free_count = 0;
    
    // Allocation phase
    while (alloc_count < 1000) {
        size_t size = rand() % 128 + 1;  // Random size between 1 and 128 bytes
        void *p = malloc(size);
        if (p) {
            ptrs[alloc_count++] = p;
        } else {
            break; // Allocation failed
        }
    }
    printf("    Allocated %d blocks of random sizes\n", alloc_count);
    
    // Shuffle pointers to randomize freeing order
    for (int i = 0; i < alloc_count; i++) {
        int j = rand() % alloc_count;
        void *temp = ptrs[i];
        ptrs[i] = ptrs[j];
        ptrs[j] = temp;
    }
    
    // Freeing phase
    while (free_count < alloc_count) {
        free(ptrs[free_count++]);
    }
    printf("    Freed all allocated blocks\n");
}

/*
 * Function: test_intentional_leak
 * --------------------------------
 * Tests the allocator's ability to detect memory leaks.
 *
 * Steps:
 * 1. Print a message indicating the start of the test.
 * 2. Allocate memory of 100 bytes using malloc(100).
 * 3. Intentionally do not free the allocated memory.
 * 4. Print a message indicating that memory was allocated and not freed.
 *
 * Note:
 * - This test is used to verify that the leak detector in the allocator
 *   correctly reports leaked memory at program exit.
 * - Uncomment the function call in 'main' to enable this test.
 *
 * Purpose:
 * - Validates the leak detection mechanism of the allocator.
 * - Ensures accurate reporting of leaked memory blocks.
 */
void test_intentional_leak() {
    printf("Test Intentional Memory Leak:\n");
    void *p = malloc(100);
    (void)p;  // Suppress unused variable warning
    // Intentionally not freeing 'p' to test leak detection
    printf("    Allocated memory and did not free it to test leak detection\n");
}

/*
 * Function: main
 * --------------
 * The main function runs all the test functions.
 *
 * Steps:
 * 1. Seed the random number generator using srand.
 * 2. Call each test function in sequence.
 * 3. Uncomment specific test functions as needed, especially those that may cause crashes.
 * 4. Return 0 to indicate successful execution.
 *
 * Purpose:
 * - Executes the suite of tests to validate the allocator's functionality.
 * - Allows for easy addition or removal of tests.
 */
int main() {
    srand((unsigned int)time(NULL)); // Seed the random number generator
    
    test_basic_allocation();
    test_exhaustive_allocation();
    test_free_coalescing();
    test_double_free();
    test_zero_size_allocation();
    test_free_null_pointer();
    // Uncomment the next line to test invalid free (may cause crash)
    // test_free_invalid_pointer();
    test_alignment();
    test_large_allocation();
    test_stress_random_sizes();
    // Uncomment the next line to test intentional memory leak
    // test_intentional_leak();
    
    return 0;
}

