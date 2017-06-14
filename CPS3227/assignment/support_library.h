#ifndef SUPPORT_LIBRARY_H_ 
#define SUPPORT_LIBRARY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>


/* 
 * Some basic global constants: 
 * DIM represents our number of dimension (2D)
 * GTERM is the gravitational constant
 * DELTA is the time step interval size
 * X is the X coordinate that is mapped to the 1st index of our force, position, velocity arrays
 * Y is the Y coordinate that is mapped to the 2nd index of our force, position, velocity arrays
*/
#define DIM 2
#define GTERM 2000.0f
#define DELTA 0.01f
#define X 0
#define Y 1


double **allocateMatrix(int rows, int cols);
void freeMatrix(double** x, int rows);
void determineParticleCount(const std::string &inputFilename, int &particleCount);
void initialise(const std::string &inputFilename, double** position, double** velocity, double *mass);
void persistPositions(const std::string &p_strFilename, const std::string &p_strDirectory, int particleCount, double** position, double* mass);


// read input file and return particle count based on line numbers
void determineParticleCount(const std::string &inputFilename, int &particleCount) {
	std::ifstream input(inputFilename.c_str());	
  	std::string line;
	if (input.is_open()) {
    	while (getline(input, line)) {
    		particleCount++;
    	}
    	input.close();
    }
}

// initialise parameters based on input file contents
void initialise(const std::string &inputFilename, double** position, double** velocity, double *mass)  {
	std::cout << "Loading particles from file: " << inputFilename << std::endl;
	std::ifstream input(inputFilename.c_str());	
  	std::string line;

	double *dblTokens;
	dblTokens = new double[3]; // same as malloc(3 * sizeof(double))
	int counter;
	int particleCounter = 0;
  	if (input.is_open()) {
    	while (getline(input, line)) {
			// remove space from line
			line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
			// parse line into tokens
			std::istringstream tokens(line);
			std::string token;
			counter = 0;
			while (getline(tokens, token, ',')) {			
      			// convert string token to double value and store in array
      			dblTokens[counter++] = atof(token.c_str());
    		}
    		// create new particle and push to vector
    		//p_bodies.push_back(Particle(dblTokens[0], dblTokens[1], dblTokens[2]));
			velocity [particleCounter][0] = 0.f;
			velocity [particleCounter][1] = 0.f;
    		mass [particleCounter] = dblTokens[0];
    		position [particleCounter][0] = dblTokens[1];    		
    		position [particleCounter++][1] = dblTokens[2];
    		
    	}
    	input.close();
 	} else
		std::cerr << "Unable to read data to file:" << inputFilename << std::endl;
		
	delete(dblTokens);

}

// allocate memory contiguously to 2D array
double **allocateMatrix(int rows, int cols) {
	double *data = (double*)malloc(rows * cols * sizeof(double));
	double **array = (double **) malloc(rows * sizeof(double*));
	for (int i = 0; i < rows; i++) {
		array[i] = &data[cols * i];
	}
	return array;
	
}

// garbage collect memory
void freeMatrix(double** x, int rows) {
	free(x[0]);
	free(x);
}

// commit particle masses and positions to file in CSV format
void persistPositions(const std::string &p_strFilename, const std::string &p_strDirectory, int particleCount, double** position, double* mass)
{
	std::string fullPath = p_strDirectory + "/" + p_strFilename;
	std::cout << "Writing to file: " << (fullPath) << std::endl;	
	std::ofstream output(fullPath.c_str());
	
	if (output.is_open())
	{	
		for (int i = 0; i < particleCount; i++)
		{
			output << 	mass[i] << ", " <<
				position[i][ X ] << ", " <<
				position[i][ Y ] << std::endl;
		}
		
		output.close();
	}
	else
		std::cerr << "Unable to persist data to file:" << p_strFilename << std::endl;
}
#endif 
