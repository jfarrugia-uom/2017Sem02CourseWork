#ifndef MPI_LIBRARY_H_ 
#define MPI_LIBRARY_H_

int** allocateMatrix(int rows, int cols);
void populateMatrix(int** matrix, int rows, int cols);
void freeMatrix(int** x, int rows);
void printMatrix(int **x, int row, int col);
int* allocateArray(int elements);
void reallocateArray(int* origArray, int newSize);
void populateArray(int* x, int elements);

#endif 
