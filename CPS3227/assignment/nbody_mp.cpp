//----------------------------------------------------------------------------------------------
//	Filename:	nbody_mp.cpp
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


typedef void (*forceFunction)(int particle, int particleCount, double** position, double** velocity, double* mass, double** force);

/* Prototypes */
void computeForce(int particle, int particleCount, double** position, double** velocity, double* mass, double** force);
void computeForceSymmetric(int particle, int particleCount, double** position, double** velocity, double* mass, double** force);
void updateBody(int particle, double** position, double** velocity, double** force);
void calculateSequential(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, short isPersist);
void calculateParallel(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, int threadCount, short isPersist);
double performSimulation(std::string fileName, int maxIteration, int attempt, short isPersist);

// directory in which to store output files
std::string output_directory;
// flag indicating whether we will be computing upper triangle nbody matrix, courtesy of Newton's third law
short is_symmetric = 1;

// compute force of particle w.r.t. all bodies interacting with it
void computeForce(int particle, int particleCount, double** position, double** velocity, double* mass, double** force) {

	double directionX = 0.f;
	double directionY = 0.f;
	double magnitude = 0.f;
	double forceX = 0.f;
	double forceY = 0.f;
	

	for (int j = 0; j < particleCount; j++) {
		if (j == particle) continue;
		// calculation offset vector on X and Y planes
		directionX = position[j][ X ] - position[particle][ X ];
		directionY = position[j][ Y ] - position[particle][ Y ];						
		magnitude = sqrt((directionX * directionX) + (directionY * directionY));
		// Limit distance term to avoid singularities
		magnitude = std::max<float>( 0.5f * (mass[j] + mass[particle]), magnitude );
		
		forceX = (mass[j] * directionX) / (magnitude * magnitude * magnitude);
		forceY = (mass[j] * directionY) / (magnitude * magnitude * magnitude);	
		
		// update force value for particle
		force[particle][ X ] += forceX;
		force[particle][ Y ] += forceY;
	}

}

// compute force of all particles but this time taking into consideration N3 ("every action has equal and opposite reaction")
void computeForceSymmetric(int particle, int particleCount, double** position, double** velocity, double* mass, double** force) {

	double directionX = 0.f;
	double directionY = 0.f;
	double magnitude = 0.f;
	double forceX = 0.f;	
	double forceY = 0.f;

	for (int j = particle + 1; j < particleCount; j++) {
		// get thread
		int me = omp_get_thread_num();
		
		directionX = position[j][ X ] - position[particle][ X ];
		directionY = position[j][ Y ] - position[particle][ Y ];						
		magnitude = sqrt((directionX * directionX) + (directionY * directionY));
		// Limit distance term to avoid singularities
		magnitude = std::max<float>( 0.5f * (mass[j] + mass[particle]), magnitude );
		
		forceX = directionX / (magnitude * magnitude * magnitude);
		forceY = directionY / (magnitude * magnitude * magnitude);		

		// every thread has its own array of particles. Memory is contiguously allocated and threads are scheduled statically.  
		// This means that every thread is accessing the same 64 byte memory chunk, thus reducing false-sharing
		int particleIndex = (particleCount * me) + particle;
		force[particleIndex][ X ] += forceX * mass[j];
		force[particleIndex][ Y ] += forceY * mass[j];
//		printf("Thread [%d], force = %.10f, X value for particle pair %d - %d = %.10f\n", me, forceX, particle, j, force[particleCount * me + particle][X]);
		
		// using Newton's 3rd (action has equal opposition reaction) we can avoid half of the calculations by negating already calculated forces
		int jIndex = (particleCount * me) + j;		
		force[jIndex][ X ] -= forceX * mass[particle];
		force[jIndex][ Y ] -= forceY * mass[particle];
		
//		printf("===> Thread [%d], X value for particle pair %d - %d = %.10f\n", me, j, particle, force[particleCount * me + j][ X ]);				
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
void calculateSequential(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, short isPersist) {
	std::stringstream fileOutput;

	printf("Calculating nbody SEQUENTIAL (Symmetric = %d) using %d bodies and %d iterations\n", is_symmetric, particleCount, iterationCount);

	// store forces
	// allocate matrix allocated memory contiguously
	double** force = allocateMatrix(particleCount, DIM);
	double* test;
	for (int timestep = 0; timestep < iterationCount; timestep++) {			
		test = &force[0][0];
		// set entire 2d array to 0;		
		test = (double*) memset(test, 0.f, particleCount * DIM * sizeof(double));
		// compute forces on particle
		for (int i = 0; i < particleCount; i++) {
			theForce(i, particleCount, position, velocity, mass, force);
		}
		// update position of particle
		for (int i = 0; i< particleCount; i++) {
			 updateBody(i, position, velocity, force);
		}
		
		// persist result if required
		if (isPersist) {			
			fileOutput.str(std::string());
			fileOutput << "nbody_" << timestep << ".txt";
			persistPositions(fileOutput.str(), output_directory, particleCount, position, mass);
		} 	
	}

	// garbage collect
	freeMatrix(force, particleCount);
}

void calculateParallel(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, int threadCount, short isPersist) {
	// set thread count for experiment
	omp_set_num_threads(threadCount);
	printf("Calculating nbody PARALLEL (Symmetric = %d) using %d bodies and %d iterations using %d threads \n", is_symmetric, particleCount, iterationCount, threadCount);
	// declare file output stream in case of persistance
	std::stringstream fileOutput;
	// store forces where each particle per thread can store its computed force - avoid having to synchronise the update portions and can exploit symmetry	
	double** force;
	// if we're exploiting symmetry, we need each each thread to access its own array of forces.  
	// Since each thread is allocated its own chunk of memory and the same particle is not being simultaneously computed by concurrent threads, we have no race conditions.
	if (is_symmetric == 1) {
		force = allocateMatrix(particleCount * threadCount, DIM);
	} else {
		force = allocateMatrix(particleCount, DIM);
	}
	
	double* test;
	for (int timestep = 0; timestep < iterationCount; timestep++) {			
		test = &force[0][0];
		// set entire 2d array to 0;  since memory was allocated contiguously we can use memset to bulk-set an entire 2D array to 0
		if (is_symmetric == 1) {
			test = (double*) memset(test, 0.f, particleCount * DIM * threadCount * sizeof(double));		
		} else {
			test = (double*) memset(test, 0.f, particleCount * DIM  * sizeof(double));		
		}
						
		#pragma omp parallel for shared(position, velocity, mass, force) schedule(static, 4)
		for (int i = 0; i < particleCount; i++) {
			theForce(i, particleCount, position, velocity, mass, force);
		}
	
		// if implementation is symmetric then second phase of the problem: reduce values of particles retained per thread to individual particles
		// static scheduling ensures that i) threads are allocated equal amount of work and adjacent particles are allocated to the same thread
		if (is_symmetric == 1) {
			#pragma omp parallel for shared(force) schedule(static, 4)
			for (int i = 0; i < particleCount; i++) {		
				for (int j = 1; j < threadCount; j++) {
					// accumulate individual thread contributions to particle location in array
					force[i][ X ] += force[(j * particleCount) + i][ X ];
					force[i][ Y ] += force[(j * particleCount) + i][ Y ];					
				}
			}
		}

		#pragma omp parallel for shared(position, velocity, mass, force) schedule(static, 4)
		for (int i = 0; i< particleCount; i++) {
			 updateBody(i, position, velocity, force);
		}

		// persist result if required
		if (isPersist) {			
			fileOutput.str(std::string());
			fileOutput << "nbody_" << timestep << ".txt";
			persistPositions(fileOutput.str(), output_directory, particleCount, position, mass);
		} 		
/*
		if (timestep == 999) {
			for (int i = 0; i < particleCount; i++) {
				printf("%0.10f, %0.10f, %0.10f\n", mass[i], position[i][X], position[i][Y]); 
			}
		}					
*/
	}

	// dispose of memory
	freeMatrix(force, particleCount * threadCount);		
}


// returns time to execute one simulation
double performSimulation(std::string fileName, int maxIteration, int attempt, short threadCount, short isPersist) {

	// our body count
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

	// set computation strategy depending on user entered parameter
	forceFunction func = (is_symmetric == 1) ? &computeForceSymmetric : &computeForce; 
	// start stopwatch!
	runtime = omp_get_wtime();

	// if thread count set to 0, then execute program sequentially 
	if (threadCount > 1)
		calculateParallel(func, particleCount, maxIteration, position, velocity, mass, threadCount, isPersist);
	else
		calculateSequential(func, particleCount, maxIteration, position, velocity, mass, isPersist);				
	// stop time
	runtime = omp_get_wtime() - runtime;
	printf("Attempt %d: Time elapsed serial: %f\n", attempt, runtime);	
	printf("--------------------------------------------------------------\n");

	// dispose of memory
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
	 * argv[5] = is symmetric ?
	 * argv[6] = do we persist?
	*/
	if (argc < 7) {
    	std::cerr << "Usage: nbody_mp  iteration_count input_file threads attempts is_symmetric [0|1] persist[1|0] " << std::endl;
    	exit(1);
  	}
		
  	maxIteration = atoi(argv[1]);
  	threadCount = atoi(argv[3]);
  	attempts = atoi(argv[4]);
  	is_symmetric = atoi(argv[5]);
	persist = atoi(argv[6]);
	output_directory = persist == 1 ? argv[7] : "foo";

	for (int i = 0 ; i < attempts; i++) {
		totalRuntime += performSimulation(argv[2], maxIteration, i, threadCount, persist);		
	}
	
	printf("Average over %d attempts: %f\n", attempts, totalRuntime/ (double) attempts);
	
	return 0;
}

