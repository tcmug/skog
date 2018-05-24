
# GENERICS
ROOT_DIR := $(ROOT)

CC = gcc

CFLAGS =
LDFLAGS =

MAIN_SRC := $(wildcard src/*.c)
MAIN_OBJ := $(patsubst src/%.c,tmp/main_%.o,$(MAIN_SRC))

skog: $(MAIN_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

tmp/main_%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf skog tmp/*
