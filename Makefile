SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET = purrify

CC = gcc
CFLAGS = -Wno-unused-parameter -Wall -Wextra -Werror -std=c99 -pedantic -Inekos-best.c/src
LDFLAGS = -pthread -ldiscord -lcurl -lcjson

ifndef PROD
CFLAGS += -g
else
CFLAGS += -O3 -march=native -mtune=native
LDLAGS += -flto=auto
endif


.ONESHELL:

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

build/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@

all: build/$(TARGET)

run: build/$(TARGET)
	cd build
	./$(TARGET)

debug: build/$(TARGET)
	cd build
	gdb -q ./$(TARGET)

leaks: build/$(TARGET)
	cd build
	valgrind --leak-check=full  ./$(TARGET)

clean:
	rm -f build/$(TARGET) build/*.log $(OBJECTS)

.PHONY: all run debug clean
