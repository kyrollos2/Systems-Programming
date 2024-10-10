# Project: malloc Lite V1.0 (mymalloc)
-------------------------------------------

## Introduction
This project implements a custom memory allocator named **mymalloc**, operating on a fixed-size memory heap of 4096 bytes. It provides custom implementations of `malloc()` and `free()`, designed to manage memory efficiently within this constrained environment. The allocator aims to mimic the functionality of the standard library's `malloc` and `free`, offering insights into the underlying mechanisms of dynamic memory management.

The project also features a comprehensive test suite, including `memgrind` and `mymalloc_small_batch_tests`, which are used to evaluate the performance, correctness, and robustness of the allocator under various conditions.

## Files Included
- **mymalloc.c**: Contains the implementation of the custom memory allocator, including `mymalloc`, `myfree`, and supporting functions.
- **mymalloc.h**: Header file exposing the user-facing features of the allocator, including macro definitions to override `malloc` and `free`.
- **memgrind.c**: Performance benchmarking program containing various workloads to assess the allocator's performance under different scenarios.
- **mymalloc_small_batch_tests.c**: Additional test program focusing on specific functionalities, edge cases, and error handling.
- **Makefile**: Script for compiling the project and managing dependencies, providing easy build commands for the test programs.
- **README.txt**: Documentation file (this file) detailing the project's purpose, features, testing strategies, and usage instructions.

## Custom Memory Allocator (mymalloc) Features
- **Fixed-Size Heap Memory**: Operates on a predefined memory pool of 4096 bytes, simulating a constrained memory environment.
- **Memory Allocation (`mymalloc`)**:
  - **Linear Allocation Strategy**: Traverses the memory heap linearly to find a suitable free block for allocation.
  - **Chunk Header Management**: Utilizes a custom `chunk_header` structure with bitfields to store metadata about each memory block, keeping per-allocation overhead minimal.
  - **Splitting of Free Chunks**: Splits larger free chunks when allocating smaller blocks, optimizing memory usage.
  - **Alignment**: Ensures that allocated memory is properly aligned to 8-byte boundaries for safe access of various data types.
- **Memory Deallocation (`myfree`)**:
  - **Coalescing of Free Chunks**: Merges adjacent free blocks during deallocation to reduce fragmentation and improve future allocation opportunities.
  - **Double Free Detection**: Implements checks to detect and prevent double free errors, enhancing stability.
- **Leak Detection**:
  - **Automatic Leak Reporting**: Registers a `leak_detector` function using `atexit()`, which scans the heap at program termination to identify and report any memory leaks.
  - **Detailed Leak Information**: Reports the total number of leaked bytes and the count of leaked memory blocks.

## Implementation Details
- **Chunk Header Structure**:
  - The `chunk_header` struct uses bitfields within a `size_t` field to store the `is_free` flag and the size of the chunk, keeping the header size to 8 bytes.
  - This compact header reduces per-allocation overhead and aligns with memory alignment requirements.
- **Memory Alignment**:
  - Allocations are aligned to 8-byte boundaries to ensure compatibility with different data types and prevent alignment-related issues.
- **Error Handling**:
  - Provides informative error messages for allocation failures, double frees, and invalid operations.
  - Exits the program in case of critical errors to prevent undefined behavior.

## Testing Strategy
### 1. Introduction
This document outlines the testing strategy for the custom memory allocator implemented in `mymalloc.c`. The testing aims to ensure correctness, efficiency, robustness, and error handling of the allocator.

### 2. Test Setup
The tests are executed through two main programs:
- **memgrind.c**: Performs workload-based performance and stress testing, measuring execution times under various memory usage patterns.
- **mymalloc_small_batch_tests.c**: Executes a series of function-specific tests to validate allocator behavior under specific conditions, edge cases, and error handling scenarios.

### 3. Testing Objectives
- **Correctness**: Ensure that allocated memory blocks do not overlap, are properly aligned, and that the requested memory size is respected.
- **Performance**: Evaluate the efficiency of memory allocation and deallocation operations under stress and typical usage scenarios.
- **Robustness**: Test the allocator's response to edge cases and incorrect usage patterns, such as zero-size allocations and maximum size requests.
- **Error Handling**: Verify that all potential errors are caught and handled appropriately, including double frees, invalid frees, and memory leaks.

### 4. Test Descriptions

#### 4.1 Basic Functionality Tests (`mymalloc_small_batch_tests.c`)
- **Test Basic Allocation and Freeing**:
  - Allocates memory for an integer, assigns a value, and frees it.
  - Verifies basic allocation and deallocation, ensuring data integrity.
- **Test Exhaustive Allocation**:
  - Allocates as many 10-byte blocks as possible until memory is exhausted.
  - Ensures the allocator handles memory exhaustion gracefully without crashing.
- **Test Free Coalescing**:
  - Allocates and frees multiple blocks in a pattern that tests coalescing of adjacent free blocks.
  - Verifies that the allocator properly merges adjacent free chunks to reduce fragmentation.

#### 4.2 Error Handling Tests (`mymalloc_small_batch_tests.c`)
- **Test Double Free**:
  - Frees the same memory block twice to test detection and handling of double free errors.
  - Ensures that the allocator prevents double frees and provides appropriate error messages.
- **Test Zero-Size Allocation**:
  - Attempts to allocate zero bytes of memory.
  - Verifies that the allocator handles zero-size allocation requests gracefully, typically returning `NULL`.
- **Test Freeing NULL Pointer**:
  - Calls `free(NULL)` to ensure that freeing a `NULL` pointer does not cause any issues.
- **Test Freeing Invalid Pointer**:
  - Attempts to free a pointer that was not allocated by `mymalloc`, such as a stack variable.
  - Checks the allocator's ability to detect and handle invalid free operations.
- **Test Memory Alignment**:
  - Allocates memory for data types requiring specific alignment (e.g., `double`).
  - Ensures that the allocator returns properly aligned memory blocks.
- **Test Large Allocation**:
  - Attempts to allocate a large block close to the total heap size.
  - Tests the allocator's handling of large allocation requests and proper error handling if the request cannot be fulfilled.
- **Test Intentional Memory Leak**:
  - Allocates memory without freeing it to validate the leak detector's ability to report leaked memory at program exit.

#### 4.3 Performance and Stress Tests (`memgrind.c`)
- **Workload 1**:
  - Allocates and immediately frees 120 blocks of 1 byte each.
  - Tests the allocator’s response time and overhead for rapid allocation and deallocation.
- **Workload 2**:
  - Allocates 120 blocks of 1 byte each, holds them, and then frees all.
  - Evaluates the allocator's performance with multiple allocations held simultaneously.
- **Workload 3**:
  - Performs random allocations and frees until all blocks are allocated and freed.
  - Simulates a realistic and dynamic memory usage pattern, testing the allocator's robustness.
- **Workload 4**:
  - Allocates 60 blocks of 20 bytes each, then frees them in reverse order.
  - Tests the allocator's efficiency with larger block sizes and stack-like access patterns.
- **Workload 5**:
  - Allocates memory in various sizes (15, 30, 45, 60, and 75 bytes), then frees them in random order.
  - Tests the allocator's handling of mixed size allocations and random deallocations.
- **Workload 6**:
  - Stress test with random allocation sizes between 1 and 128 bytes, with random allocation and freeing patterns.
  - Evaluates the allocator under heavy stress and unpredictable usage.
- **Workload 7**:
  - Attempts to allocate a large block close to the maximum possible size.
  - Tests how the allocator handles maximum size allocation requests.
- **Workload 8**:
  - Attempts to allocate zero bytes and then frees the result.
  - Ensures graceful handling of zero-size allocations.

### 5. Additional Testing Considerations
- **Memory Leak Detection**:
  - The `leak_detector` function automatically reports any memory leaks detected at program exit.
  - Tests include intentional memory leaks to validate the leak detection mechanism.
- **Error Handling Verification**:
  - Tests are designed to trigger and verify proper error handling for invalid operations.
  - The allocator provides informative error messages and prevents undefined behavior.
- **Alignment Verification**:
  - Tests ensure that memory alignment requirements are met, preventing potential issues with misaligned access.
- **Stress Testing and Robustness**:
  - Randomized and stress tests simulate real-world usage and heavy loads to uncover potential weaknesses.

## Usage Instructions
### Compiling the Allocator and Test Programs
Use the provided `Makefile` to compile the allocator and test programs.

- **Commands**:
  - `make memgrind`: Compiles the `memgrind` test program.
  - `make small_batch_tests`: Compiles the `mymalloc_small_batch_tests` program.
  - `make all`: Compiles both test programs.
  - `make clean`: Cleans up compiled object files and executables.

### Running Tests
- **Executing memgrind**:
  - Run `./memgrind` to execute performance and stress tests.
  - The program outputs average execution times for each workload.
- **Executing Small Batch Tests**:
  - Run `./small_batch_tests` to perform functional and error handling tests.
  - Observe the console output for detailed results of each test.

### Interpreting Results
- **Output Analysis**:
  - Check for any error messages or unexpected behavior in the output.
  - For intentional error tests (e.g., double free), ensure that the allocator reports errors appropriately.
- **Leak Detector Output**:
  - Review the leak detector's report at program exit for any detected memory leaks.
  - Ensure that all allocated memory is properly freed, except in tests designed to leak memory.


*End of README.txt*
