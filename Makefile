SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET = purrify

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g
LDFLAGS =

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all run clean
