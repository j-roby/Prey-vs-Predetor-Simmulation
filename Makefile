CFLAGS = -Wall -pthread -pedantic -std=gnu99

.PHONY: all
all: Simmulation

Simmulation: simm.c
		gcc $(CFLAGS) -g simm.c -o Simmulation
