CFLAGS = -Wall -pthread -pedantic -std=gnu99 -fopenmp
.PHONY: all
all: Simulation_serial

Simulation_openmp: simm_openMP.c
        gcc $(CFLAGS) -g sim_openMP.c -o Simulation_openmp
Simulation_serial: simm.c
        gcc $(CFLAGS) -g sim.c -o Simulation_serial                                                       
