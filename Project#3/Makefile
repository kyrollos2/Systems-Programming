# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Directories
SRCDIR = .
OBJDIR = ./obj
BINDIR = ./bin

# Project files
SRCFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))
TARGET = $(BINDIR)/mysh

# Default rule
all: $(TARGET)

# Linking the target
$(TARGET): $(OBJFILES) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Create binary directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

# Clean up build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Rebuild everything
rebuild: clean all

# Phony targets
.PHONY: all clean rebuild
