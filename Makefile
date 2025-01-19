CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LIBS = -lmongoc-1.0 -lbson-1.0 -lfuse3
INCLUDES = -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -I/usr/include/fuse3

# Output binary name
OUTPUT = sbdfs

# Source files
SRC = ./src/sbdfs.c ./src/mongo.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(OUTPUT)

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link object files to create the binary
$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(OUTPUT)

# Clean the build files
clean:
	rm -f $(OBJ) $(OUTPUT)
