#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include "omp.h"
 
void helloWorld(int rank, int size) {

	printf( "Hello world from process %d of %d\n", rank, size );
}

void printError(int errorCode, int myRank) {
	char errorString[BUFSIZ];
	int errorStringLen, errorClass;

	MPI_Error_class(errorCode, &errorClass);
	MPI_Error_string(errorClass, errorString, &errorStringLen);
	fprintf(stderr, "%3d: %s\n", myRank, errorString);
	MPI_Error_string(errorCode, errorString, &errorStringLen);
	fprintf(stderr, "%3d: %s\n", myRank, errorString);
	MPI_Abort(MPI_COMM_WORLD, errorCode);	
}

int main (int argc,char **argv) {
	int rank, size;
	int number;
	int errorCode;

	omp_set_num_threads(2);

	// initialize MPI environment
	MPI_Init (&argc, &argv);
	// get process id within MPI_COMM_WORLD
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	// get size of cluster
	MPI_Comm_size (MPI_COMM_WORLD, &size);   
	// setup error handling
	MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
	helloWorld(rank, size);
	if (rank == 0) {
		number = -1;
		// send a number to other processes
		#pragma omp parallel for private(errorCode, rank)
		for (int i = 0; i < size; i++) {		
			errorCode = MPI_Send(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			if (errorCode != MPI_SUCCESS) {		
				printError(errorCode, rank);
			}
		}
		errorCode = MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (errorCode != MPI_SUCCESS) {
			printError(errorCode, rank);
		}
		printf("Process %d received number %d from process 0\n", rank, number);
		
	} else if (rank > 0) {
		errorCode = MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (errorCode != MPI_SUCCESS) {
			printError(errorCode, rank);
		}
		printf("Process %d received number %d from process 0\n", rank, number);
	}

  	MPI_Finalize();  
	return 0;
}


