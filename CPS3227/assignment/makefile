MPICXX?=mpicxx
CC=g++ 
CFLAGS=-std=c++11 -fopenmp
CFLAGS_OPTO = ${CFLAGS} -O2
FLAG_NOMP=-std=c++11 

nbody_mpi_opto:
	${MPICXX} ${CFLAGS_OPTO}  nbody_mpi.cpp support_library.h -o nbody_mpi_optimised

nbody_mpi:
	${MPICXX} ${CFLAGS}  nbody_mpi.cpp support_library.h -o nbody_mpi

all: nbody_mp_opto

nbody_mp: 
	${CC} ${CFLAGS} nbody_mp.cpp support_library.h -o nbody_mp

nbody_mp_opto:
	${CC} ${CFLAGS_OPTO} nbody_mp.cpp support_library.h -o nbody_mp_optimised

clean:
	rm -f nbody*.txt
