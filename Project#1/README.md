CS214 Systems Programming. My little malloc() by Neer Patel and Kyrollos Girgis 
This project implements a simple custom memory allocator (mymalloc) that operates on a fixed-size memory heap. The project also includes memgrind, a test program designed to analyze the performance of the custom allocator under different memory workloads.
Files:
mymalloc.c: Contains the implementation of the custom memory allocator, including the mymalloc() and myfree() functions, which replicate malloc() and free() from the standard library, but operate on a fixed memory pool.
memgrind.c: Contains three workloads that test the performance of the memory allocator by simulating different memory usage patterns.
Makefile: Provides instructions to compile the project and manage object files.

Workloads:

The memgrind program contains three test workloads:

Workload 1:
Allocates and immediately frees 120 blocks of 1 byte each.
Tests basic memory allocation and deallocation performance.

Workload 2:
Allocates 120 blocks of 1 byte each, stores pointers to them, and then frees them all.
Tests bulk memory allocation followed by bulk deallocation.

Workload 3:
Randomly allocates or frees memory until all 120 blocks are allocated and freed.
Simulates a realistic, dynamic memory usage pattern that mimics the behavior of real-world programs.
Each workload is run 50 times, and the average execution time in microseconds is calculated and displayed for each workload.

Custom Memory Allocator (mymalloc)
The custom allocator (mymalloc.c) is designed to manage memory within a fixed-size memory pool of 4096 bytes, split into chunks. It tracks allocated and free memory chunks, and supports the following features:

Memory allocation (mymalloc):
The requested memory size is aligned to 8 bytes for efficiency.
The allocator checks for free chunks in the memory pool and splits larger chunks if necessary.

Memory deallocation (myfree):
Frees a block of memory and marks it as available.
Merges adjacent free blocks to prevent fragmentation.

Leak detection:
A function leak_detector is registered with atexit() to detect any memory leaks when the program exits.
The program reports any memory chunks that were allocated but not freed.

How it Works:

Initialization:
The memory heap is initialized as a continuous block of 4096 bytes.
Memory requests are served by splitting this block into smaller chunks as needed.

Memory Management:
The allocator keeps track of each memory chunk's size and whether it's free or allocated.
When memory is freed, the allocator merges adjacent free chunks to reduce fragmentation.

Leak Detection:
Before the program exits, it checks if any allocated memory blocks were not freed and reports them.

"make clean"
This will delete the mymalloc.o and memgrind files.
