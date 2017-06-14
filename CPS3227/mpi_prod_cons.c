#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include "omp.h"

#include "mpi_library.h"

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
	
	int BUFFER_SIZE = 1000;
	int* buffer;
	int* consumerBuffer;
	int stopSignal = 0;

//	omp_set_num_threads(2);

	// initialize MPI environment
	MPI_Init (&argc, &argv);
	// get process id within MPI_COMM_WORLD
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	// get size of cluster
	MPI_Comm_size (MPI_COMM_WORLD, &size);   
	// setup error handling
	MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

	buffer = allocateArray(BUFFER_SIZE);
	int result = 0;		
	int consumerTotal = 0;
//	while(1) {	
		// assign rank = 0 as producer	
		if (rank == 0) {
			populateArray(buffer, BUFFER_SIZE);
			for (int i = 0; i < BUFFER_SIZE; i++) {											
				// route message to other proceses except the producer process
				int destination = (i % (size - 1)) + 1;
				printf("Producer %d sending[%d] to destination = %d\n", rank, buffer[i], destination);
				errorCode = MPI_Send(&buffer[i], 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
				if (errorCode != MPI_SUCCESS) printError(errorCode, rank);				
//				if (destination % (size-1)  == 0) printf("====> Process with rank [%d] at barrier\n", rank);
				// work in lock-step: suspend producer until all consumer processes reach barrier, then resume
				if (destination % (size-1)  == 0) MPI_Barrier(MPI_COMM_WORLD);								
//				if (destination % (size-1)  == 0) printf("<==== Process with rank [%d] passed barrier\n", rank);
			}
			// send stop signal
			printf("**** sending stop signal from producer\n");
			stopSignal = -1;
			for (int i = 1; i <=size; i++) {
				errorCode = MPI_Send(&stopSignal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

		// this is our consumer part
		} else  {
			int counter = 0;
			while (1) {					
				errorCode = MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);		
				if (number == -1) break;
				if (errorCode != MPI_SUCCESS) printError(errorCode, rank);
//				sleep(1);
				printf("Consumer %d received number %d from process 0\n", rank, number);
				buffer[counter++] = number;
//				printf("====> Process with rank [%d] at barrier\n", rank);
				MPI_Barrier(MPI_COMM_WORLD);															
//				printf("<==== Process with rank [%d] passed barrier\n", rank);						
			}
			printf("Received stop signal...exiting.\n"); 
			printf("Consumer with rank %d counter value = %d\n", rank, counter);
			for (int i = 0; i < counter; i++){
				consumerTotal += buffer[i];
			}
			printf("Consumer %d subtotal = %d\n", rank, consumerTotal);						
		}
		
		MPI_Reduce(&consumerTotal, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		if (rank == 0) {
			printf("Total sum = %d\n", result);
		}
				
//	}
  	
  	MPI_Finalize();  
  	free(buffer);
	return 0;
}


