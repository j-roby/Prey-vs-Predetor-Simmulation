CFLAGS = -Wall -pthread -pedantic -std=gnu99 -fopenmp
.PHONY: all
all: Simmulation_openmp Simmulation_serial_

Simmulation_openmp: simm_openMP.c
        gcc $(CFLAGS) -g simm_openMP.c -o Simmulation_openmp
Simmulation_serial: simm.c
        gcc $(CFLAGS) -g simm.c -o Simmulation_serial
~                                                       
