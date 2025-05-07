# Makefile for compiling the shell program

# The compiler to use
CC = gcc

# Compiler flags (Options for the compiler)
CFLAGS = -Wall -g

# Name of the final target program
TARGET = shell

# Source file
SRC = shell.c

# The target rule: 'shell'
$(TARGET): $(SRC)
	# Compile the source file into the target program using gcc
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Clean rule to remove the compiled program
clean:
	# Remove the generated executable
	rm -f $(TARGET)