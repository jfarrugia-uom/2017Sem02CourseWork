#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "omp.h"

#include "mpi_library.h"

/*void passArrayByReference(int* array, int rowSize) {

	for (int i = 0; i < rowSize; i++) {
		printf("Value at %d row is: %d\n", i, array[i]);	
	}
	int x = *(array + 1);
	*(array + 1) = *array;
	*array = x;
}*/


int** matrixProduct(int** matrixA, int** matrixB, int row, int col) {
	int** result = allocateMatrix(row, col);
	for (int i = 0; i < row; i++) {	
		for (int j = 0; j < col; j++) {
			result[i][j] = 0;
			for (int k = 0; k < col; k++) {
//				printf("Location matrixA [%d, %d] = %d; location matrix B [%d,%d] = %d\n", i, k, matrixA[i][k], k, j, matrixB[k][j]);   
				result[i][j] += matrixA[i][k] * matrixB[k][j];				
			}
		}		
	}
	
	return result;
}

int** matrixProductParallel(int** matrixA, int** matrixB, int row, int col, int threadCount) {
	int** result = allocateMatrix(row, col);
	omp_set_num_threads(threadCount);
	int tmp = 0;
	#pragma omp parallel for private(tmp)
	for (int i = 0; i < row; i++) {
		printf("Thread [%d]; i = %d\n",  omp_get_thread_num(), i);
		for (int j = 0; j < col; j++) {
//			printf("Inner loop - in Thread [%d]\n",  omp_get_thread_num());			
			tmp = 0;
			for (int k = 0; k < col; k++) {	
//				printf("Thread [%d]: Location matrixA [%d, %d] = %d; location matrix B [%d,%d] = %d\n", omp_get_thread_num(), i, k, matrixA[i][k], k, j, matrixB[k][j]);   
				tmp += matrixA[i][k] * matrixB[k][j];
			}
			result[i][j] = tmp;
		}		
	}
	
	return result;
}

int** matrixProductParallelDebarro(int** matrixA, int** matrixB, int row, int col, int threadCount) {
	int** result = allocateMatrix(row, col);
	omp_set_num_threads(threadCount);

	int total = row * col;
	#pragma omp parallel for 
	for (int xy = 0; xy < total; xy++) {
		int x = xy / row;
		int y = xy % col;

		for (int k = 0; k < col; k++) {	
			result[x][y] = result[x][y] + matrixA[x][k] * matrixB[k][y];
		}
	}
	
	return result;
}

int** matrixProductParallelCollapse(int** matrixA, int** matrixB, int row, int col, int threadCount) {
	int** result = allocateMatrix(row, col);
	omp_set_num_threads(threadCount);
	int tmp = 0;
	#pragma omp parallel for private (tmp) collapse(2)
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {						
			for (int k = 0; k < col; k++) {	
				tmp += matrixA[i][k] * matrixB[k][j];
			}
			result[i][j] = tmp;
			tmp = 0;
		}		
	}	
	return result;
}


void usePointerAs2DMatrix() {
	int rows, cols;
	int **x, **y, **z, **_z = NULL;
	// set array size and allocate memory
	rows = cols = 3;
	x = allocateMatrix(rows, cols);
	y = allocateMatrix(rows, cols);
	// populate matrices based on random numbers from 1 to 100
	populateMatrix(x, rows, cols);	
	populateMatrix(y, rows, cols);	
	
	printMatrix(x, rows, cols);
	int* test = &x[0][0];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("value of test at %d,%d = %x\n", i, j, &x[i][j]);
		}		
	}
	
	printf("test = %d\n", *test);
	printf("Address x[0] = %x; value = %d\n", x[1], *(x[1]+0));
	
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			//printf("index = %d\n", i * rows + j);
			printf("Address [%d][%d] = %x; result = %d\n", i, j, test + (i * rows +j), *(test + (i * rows +j)));
		}
	} 
	freeMatrix(x, rows);
	freeMatrix(y, rows);

}

int main() {
	
/*	int y[2] = {1,2};
	passArrayByReference(&y[0], 2);*/
	// same as 	passArrayByReference(y, 2);	

//	doTrapezoidStuff();

	usePointerAs2DMatrix();
	double runtime;

	int rows, cols;
	int **x, **y, **z, **_z = NULL;
	// set array size and allocate memory
	rows = cols = 1000;
	x = allocateMatrix(rows, cols);
	y = allocateMatrix(rows, cols);
	// populate matrices based on random numbers from 1 to 100
	populateMatrix(x, rows, cols);	
	populateMatrix(y, rows, cols);		
	
	printf("Printing MatrixA...\n");	
//	printMatrix(x, rows, cols);
	printf("Printing MatrixB...\n");
//	printMatrix(y, rows, cols);

	int THREAD_COUNT = 2;
	
	// compute sequential matrix product
	printf("Compute sequential matrix product:\n");
	runtime = omp_get_wtime();
	_z = matrixProduct(x, y, rows, cols);
	runtime = omp_get_wtime() - runtime;
	printf("Time elapsed serial: %f\n", runtime);	

	// compute parallel matrix product
	printf("Compute parallel matrix product (%d threads):\n", THREAD_COUNT);

	runtime = omp_get_wtime();
	z = matrixProductParallelCollapse(x, y, rows, cols, THREAD_COUNT);
	runtime = omp_get_wtime() - runtime;
	printf("Time elapsed parallel: %f\n", runtime);

/*
	printMatrix(_z, rows, cols);
	printf("---------------------------------------------------------------\n");	
	printMatrix(z, rows, cols);*/

	// free matrix memory
	freeMatrix(x, rows);
	freeMatrix(y, rows);
	freeMatrix(z, rows);
	freeMatrix(_z, rows);
}
