#include <stdio.h>
#include <stdlib.h>
#include <cstring.h>
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
	int errorCode;

	int rows, cols;
	int **x, **y;

	int *matrixRow, *partialProduct;
//	omp_set_num_threads(2);

	// initialize MPI environment
	MPI_Init (&argc, &argv);
	// get process id within MPI_COMM_WORLD
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	// get size of cluster
	MPI_Comm_size (MPI_COMM_WORLD, &size);   
	// setup error handling
	MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

	rows = cols = size;	
	y = allocateMatrix(rows, cols);
	x = allocateMatrix(rows, cols);
	matrixRow = allocateArray(rows);
	partialProduct = allocateArray(rows);

	if (rank == 0) {				
		populateMatrix(x, rows, cols);
		populateMatrix(y, rows, cols);
		printf("*** Matrix x :-\n");
		printMatrix(x, rows, cols);
		
		printf("*** Matrix y :-\n");
		printMatrix(y, rows, cols);
			
	} 
		
	// broadcast matrix Y to all peers
	MPI_Bcast(&y[0][0], rows * cols, MPI_INT, 0, MPI_COMM_WORLD);
	// broadcast 
	// scatter individual rows of matrix to peers
	MPI_Scatter(&x[0][0], rows, MPI_INT, matrixRow, rows, MPI_INT, 0, MPI_COMM_WORLD); 
/*
	for (int i = 0; i < rows; i++) {
		printf("Process [%d]: element at [%d] = %d\n", rank, i, *(matrixRow+i));
	}
*/	
		
	// compute partial product of matrix
	for (int i = 0; i < rows; i++) {
		partialProduct[i] = 0;
		for (int j = 0; j < cols; j++) {
			partialProduct[i] += matrixRow[j] * y[j][i]; 
		}
	}
/*	
	for (int i = 0; i < rows; i++) {
		printf("Process [%d]: element at [%d] = %d\n", rank, i, *(partialProduct+i));
	}
*/	
	int *z = NULL;
	if (rank == 0) z = allocateArray(rows * cols);
	
	MPI_Gather(partialProduct, rows, MPI_INT, z, rows, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				printf("[%d][%d] result = %d\n", i, j,  *(z + (i * rows +j)));
			}
		}
	}
	

	freeMatrix(x, rows); 
	free(z); 		
	freeMatrix(y, rows);
	free(matrixRow);
	free(partialProduct);

	MPI_Finalize();  
}


