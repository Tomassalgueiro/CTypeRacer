# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags libcurl 2>/dev/null)
LDFLAGS = $(shell pkg-config --libs libcurl 2>/dev/null || echo "-lcurl")

TARGET = typeracer

SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Rebuild from scratch
re: clean all

.PHONY: all clean re
