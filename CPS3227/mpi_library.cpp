#include "mpi_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


/*int** allocateMatrix(int rows, int cols) {
	// obtain values for rows & cols 
	// allocate the array 
	// pointer size is 8 bytes = 64 bits
	int** x;
	x = malloc(rows * sizeof *x);
	for (int i = 0; i < rows; i++) {
		x[i] = malloc(cols * sizeof * x[i]);
	}
	return x;
}*/

int ** allocateMatrix(int rows, int cols) {
	int *data = (int*)malloc(rows * cols * sizeof(int));
	int **array = (int **) malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; i++) {
		array[i] = &data[cols * i];
	}
	return array;
	
}

void populateMatrix(int** matrix, int rows, int cols) {
	printf("Populating matrix\n");
	sleep(1);
	srand ( time(NULL) );
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = rand() % 100;
		}
	}
	
}

/*void freeMatrix(int** x, int rows) {
	// deallocate the array 
	for (int i = 0; i < rows; i++) {
		free(x[i]);
	}
	free(x);	
}*/

void freeMatrix(int** x, int rows) {
	free(x[0]);
	free(x);
}



void printMatrix(int **x, int row, int col) {
	for (int i = 0; i < row; i++) {
		for (int j =0; j < row; j++) {
			printf("Value at [%d,%d] = %d\n", i, j, x[i][j]);
		}
	}
}

int* allocateArray(int elements) {
	int* x;
	x = (int*)malloc(elements * sizeof (int));
	return x;
}

void reallocateArray(int* origArray, int newSize) {
	int* newArray;
	newArray = (int*)realloc(origArray, newSize * sizeof(int));
	origArray = newArray;
}

void populateArray(int* x, int elements) {
	srand ( time(NULL) );
	for (int i = 0; i < elements; i++) {		
		x[i] = rand() % 100;
		printf("x[%d] = %d\n", i, x[i]);
	//	sleep(1);
	}
}


