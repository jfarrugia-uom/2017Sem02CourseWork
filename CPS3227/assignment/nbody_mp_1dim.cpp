//----------------------------------------------------------------------------------------------
//	Filename:	nbody_seq_jfarr.cpp
//	Author:		James Farrugia (based on Keith Bugeja and https://www.cs.usask.ca/~spiteri/CMPT851/notes/nBody.pdf)
//----------------------------------------------------------------------------------------------
//  CPS3227 assignment for academic year 2017/2018:
//	nbody simulation using both sequential and shared memory (OpenMP) constructs
//----------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include "omp.h"

#include "support_library.h"


#define DIM 2
#define GTERM 2000.0f
#define DELTA 0.01f
#define X 0
#define Y 1


typedef void (*forceFunction)(int particle, int particleCount, double** position, double** velocity, double* mass, double** force);

/* Prototypes */
void computeForce(int particle, int particleCount, double** position, double** velocity, double* mass, double** force);
void computeForceSymmetric(int particle, int particleCount, double** position, double** velocity, double* mass, double** force);
void updateBody(int particle, double** position, double** velocity, double** force);
void calculateSequential(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass);
void calculateParallel(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, int threadCount, short isPersist);
double performSimulation(std::string fileName, int maxIteration, int attempt, short isPersist);

std::string output_directory;

// compute force of particle w.r.t. all bodies interacting with it
void computeForce(int particle, int particleCount, double** position, double** velocity, double* mass, double** force) {

	double directionX = 0.f;
	double directionY = 0.f;
	double magnitude = 0.f;
	double forceX = 0.f;
	double forceY = 0.f;
	
//	printf("Doing particle %d\n", particle);
	for (int j = 0; j < particleCount; j++) {
		if (j == particle) continue;
		directionX = position[j][ X ] - position[particle][ X ];
		directionY = position[j][ Y ] - position[particle][ Y ];						
		magnitude = sqrt((directionX * directionX) + (directionY * directionY));
		// Limit distance term to avoid singularities
		magnitude = std::max<float>( 0.5f * (mass[j] + mass[particle]), magnitude );
//		printf("Particle [%d:%d] = directionX = %.8f, directionY = %.8f, magnitude = %.8f, mass = %.8f ", particle, j, directionX, directionY, magnitude, mass[j]);
		
		forceX = (mass[j] * directionX) / (magnitude * magnitude * magnitude);
		forceY = (mass[j] * directionY) / (magnitude * magnitude * magnitude);	
		
//		printf("force X = %0.8f, force Y = %0.8f\n", forceX, forceY);
		force[particle][ X ] += forceX;
		force[particle][ Y ] += forceY;
	}
//	printf("-----------------------------------\n");
}

void computeForceSymmetric(int particle, int particleCount, double** position, double** velocity, double* mass, double** force) {

	double directionX = 0.f;
	double directionY = 0.f;
	double magnitude = 0.f;
	double forceX = 0.f;	
	double forceY = 0.f;

	for (int j = particle + 1; j < particleCount; j++) {
		int me = omp_get_thread_num();

		directionX = position[j][ X ] - position[particle][ X ];
		directionY = position[j][ Y ] - position[particle][ Y ];						
		magnitude = sqrt((directionX * directionX) + (directionY * directionY));
		// Limit distance term to avoid singularities
		magnitude = std::max<float>( 0.5f * (mass[j] + mass[particle]), magnitude );
		//printf("Thread [%d]: Particle [%d:%d] = directionX = %.8f, directionY = %.8f, magnitude = %.8f, mass = %.8f\n ", me, particle, j, directionX, directionY, magnitude, mass[j]);					
		
		forceX = directionX / (magnitude * magnitude * magnitude);
		forceY = directionY / (magnitude * magnitude * magnitude);		
//		printf("force X = %0.8f, force Y = %0.8f\n", forceX, forceY);

//		printf("Store %0.8f in force[%d][%d] ", forceX, particle, 2 * me + X);
//		printf(" Store %0.8f in force[%d][%d]\n", forceY, particle, 2 * me + Y);
	

//		printf("Thread [%d] - Accessing memory location for particle %d (X,Y): %x; %x\n", me, particle, &force[particleCount * me + particle][ X ], &force[particleCount * me + particle][ Y ]);
		force[particleCount * me + particle][ X ] += forceX * mass[j];
		force[particleCount * me + particle ][ Y ] += forceY * mass[j];
		
//		printf("Store %0.8f in force[%d][%d] ", -forceX, j, 2 * me + X);
//		printf(" Store %0.8f in force[%d][%d]\n", -forceY, j, 2 * me + Y);
		// using Newton's 3rd (action has equal opposition reaction) we can avoid half of the calculations by negating already calculated forces

//		printf("\tSymmetric Thread [%d] - Accessing memory location for particle %d (X,Y): %x; %x\n", me, j, &force[particleCount * me + j][ X ], &force[particleCount * me + j][ Y ]);
		force[particleCount * me + j][ X ] -= forceX * mass[particle];
		force[particleCount * me + j][ Y ] -= forceY * mass[particle];
	}
}

// update velocity and position of body at timestep
void updateBody(int particle, double** position, double** velocity, double** force) {
	velocity[particle][ X ] += GTERM * force[particle][ X ] *  DELTA;
	velocity[particle][ Y ] += GTERM * force[particle][ Y ] *  DELTA;
	position[particle][ X ] += velocity[particle][ X ] * DELTA;
	position[particle][ Y ] += velocity[particle][ Y ] * DELTA;		
}

// execute iterations to calculate future position of bodies in our set
void calculateSequential(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass) {
	printf("Calculating nbody SEQUENTIAL using %d bodies and %d iterations\n", particleCount, iterationCount);

	// store forces
	double** force = allocateMatrix(particleCount, DIM);
	double* test;
	for (int timestep = 0; timestep < iterationCount; timestep++) {			
		test = &force[0][0];
		// set entire 2d array to 0;		
		test = (double*) memset(test, 0.f, particleCount * DIM * sizeof(double));
		for (int i = 0; i < particleCount; i++) {
			theForce(i, particleCount, position, velocity, mass, force);
		}

		for (int i = 0; i< particleCount; i++) {
			 updateBody(i, position, velocity, force);
		}
	}
		/* 2d array can be referenced as *(force[i] + j); force[i][j]; *(*(force+i) + j); if referenced as *p also *(p + (i * rows + j)) */

/*	for (int i = 0; i < particleCount; i++) 
		printf("Final position for particle %d at timestep %d = %.6f, %.6f\n", i, iterationCount, position[i][X], position[i][Y]);*/

	freeMatrix(force, particleCount);
}

void calculateParallel(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, int threadCount, short isPersist) {
	// set thread count for experiment
	omp_set_num_threads(threadCount);
	printf("Calculating nbody PARALLEL using %d bodies and %d iterations using %d threads \n", particleCount, iterationCount, threadCount);
	// declare file output stream in case of persistance
	std::stringstream fileOutput;
	// store forces where each particle per thread can store its computed force - avoid having to synchronise the update portions and can exploit symmetry
	double** force = allocateMatrix(particleCount * threadCount, DIM);
	double* test;
	for (int timestep = 0; timestep < iterationCount; timestep++) {			
		test = &force[0][0];
		// set entire 2d array to 0;  since memory was allocated contiguously we can use memset to bulk-set an entire 2D array to 0
		test = (double*) memset(test, 0.f, particleCount * DIM * threadCount * sizeof(double ));		
/*		for (int i = 0; i < particleCount; i++) {
			for(int j = 0; j < threadCount; j++) {
				printf("X coord: location [%d][%d]; address %x\n", i, j + 0, force + (i * particleCount + (2 * j + 0)));
				printf("Y coord: location [%d][%d]; address %x\n", i, j + 0, force + (i * particleCount + (2 * j + 1)));
			}
		}*/
/*		
		for (int i = 0; i < particleCount; i++) {
			for (int j = 0; j < DIM * threadCount; j++) {
				//printf("index = %d\n", i * rows + j);
				printf("Address [%d][%d] = %x; result = %3.f; test address = %x\n", i, j, &force[i][j], force[i][j], (test + (i * (threadCount * DIM) + j)));
			}
		}		*/
		#pragma omp parallel for shared(position, velocity, mass, force) schedule(static, 4)
		for (int i = 0; i < particleCount; i++) {
//			printf("Thread [%d] Running i = %d\n", omp_get_thread_num(), i);
			theForce(i, particleCount, position, velocity, mass, force);
		}
		
		// second phase of the problem
		#pragma omp parallel for shared(force) schedule(static, 4)
		for (int i = 0; i < particleCount; i++) {		
			for (int j = 1; j < threadCount; j++) {
				// accumulate individual thread contributions to particle location in array
				//printf("Thread [%d-%d] X Force for body %d = %.8f\n", omp_get_thread_num(),j, i,force[i][2 * j + 0]);
				//printf("Thread [%d-%d] Y Force for body %d = %.8f\n", omp_get_thread_num(),j, i,force[i][2 * j + 1]);			
				force[i][ X ] += force[j * particleCount + i][ X ];
				force[i][ Y ] += force[j * particleCount + i][ Y ];
			}
		}
		
//		#pragma omp parallel for shared(position, velocity, force)
		for (int i = 0; i< particleCount; i++) {
			 updateBody(i, position, velocity, force);
		}
		
		if (isPersist) {			
			fileOutput.str(std::string());
			fileOutput << "nbody_" << timestep << ".txt";
			persistPositions(fileOutput.str(), output_directory, particleCount, position, mass);
		} 	
	}
		/* 2d array can be referenced as *(force[i] + j); force[i][j]; *(*(force+i) + j); if referenced as *p also *(p + (i * rows + j)) */

/*	for (int i = 0; i < particleCount; i++) 
		printf("Final force for particle %d at timestep %d = %.6f, %.6f\n", i, iterationCount, position[i][X], position[i][Y]);*/

	freeMatrix(force, particleCount * threadCount);		
}


// returns time to execute one simulation
double performSimulation(std::string fileName, int maxIteration, int attempt, short threadCount, short isPersist) {

	// our IVP body count
	int particleCount = 0;
	double runtime;
	
	// make first pass on input file to determine body count after which arrays can be initialised
	determineParticleCount(fileName, particleCount);
	printf("Particle count = %d\n", particleCount);
	// initialise position, velocity and mass arrays
	double** position = allocateMatrix(particleCount, DIM);
	double** velocity = allocateMatrix(particleCount, DIM);
	double* mass = (double*)malloc(particleCount * sizeof(double));

	// initialise bodies based on input file
	initialise(fileName, position, velocity, mass);

	// set computation strategy
//	forceFunction func = &computeForce;
	forceFunction func = &computeForceSymmetric;
	runtime = omp_get_wtime();

	if (threadCount > 1)
		calculateParallel(func, particleCount, maxIteration, position, velocity, mass, threadCount, isPersist);
	else
		calculateSequential(func, particleCount, maxIteration, position, velocity, mass);				

	runtime = omp_get_wtime() - runtime;
	printf("Attempt %d: Time elapsed serial: %f\n", attempt, runtime);	
	printf("--------------------------------------------------------------\n");


	freeMatrix(position, particleCount);
	freeMatrix(velocity, particleCount);
	free(mass);
	return runtime;
}



int main(int argc, char **argv)
{	
	// max number of iterations specified by user
	int maxIteration = 0;
	int attempts = 0;
	short threadCount = 0;
	short persist = 0;
	
	// timing variable 
	double totalRuntime = 0.f;
	
	/* Parameters:
	 * argv[1] = iterations
	 * argv[2] = input_file
	 * argv[3] = thread_count
	 * argv[4] = attempts
	 * argv[5] = do we persist?
	*/
	if (argc < 6) {
    	std::cerr << "Usage: nbody_mp_jfarr  iteration_count input_file threads attempts persist[1|0]" << std::endl;
    	exit(1);
  	}
		
  	maxIteration = atoi(argv[1]);
  	threadCount = atoi(argv[3]);
  	attempts = atoi(argv[4]);
	persist = atoi(argv[5]);
	output_directory = persist == 1 ? argv[6] : "foo";

	for (int i = 0 ; i < attempts; i++) {
		totalRuntime += performSimulation(argv[2], maxIteration, i, threadCount, persist);		
	}
	
	printf("Average over %d attempts: %f\n", attempts, totalRuntime/ (double) attempts);

//	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		
	return 0;
}


