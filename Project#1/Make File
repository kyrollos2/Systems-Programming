CC = gcc
CFLAGS = -g -Wall

# Specify the directory for clearer management
DIR = Project#1

# Objects and executables
LIB_OBJS = $(DIR)/mymalloc.o
TEST_PROGRAMS = $(DIR)/memgrind $(DIR)/mymalloc_small_batch_tests

all: $(TEST_PROGRAMS)

# Compile the allocator library
$(DIR)/mymalloc.o: $(DIR)/mymalloc.c $(DIR)/mymalloc.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the memgrind test program
$(DIR)/memgrind: $(DIR)/memgrind.c $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile the allocator tests program
$(DIR)/mymalloc_small_batch_tests: $(DIR)/mymalloc_small_batch_tests.c $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up compiled files
clean:
	rm -f $(TEST_PROGRAMS) $(LIB_OBJS)
