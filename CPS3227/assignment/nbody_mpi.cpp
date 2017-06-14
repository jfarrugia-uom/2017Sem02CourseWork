//----------------------------------------------------------------------------------------------
//	Filename:	nbody_mpi.cpp
//	Author:		James Farrugia (based on Keith Bugeja and https://www.cs.usask.ca/~spiteri/CMPT851/notes/nBody.pdf)
//----------------------------------------------------------------------------------------------
//  CPS3227 assignment for academic year 2017/2018:
//	nbody implementation using MPI distributed memory constructs
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
#include <mpi.h>
#include "omp.h"
#include "support_library.h"


typedef void (*forceFunction)(int particle, int particleCount, double** position, double* mass, double** force);

/* function prototypes */
void printError(int errorCode, int myRank);
// note remove velocity reference - not needed to compute forces
void computeForce(int particle, int particleCount, double** position, double* mass, double** force);
void updateBody(int particle, int particleCount, double** position, double** velocity, double** force);
void calculateSequential(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, short isPersist);
void calculateParallel(forceFunction theForce, int particleCount, int iterationCount, double** position, double** velocity, double* mass, int threadCount, short isPersist);
double performSimulation(std::string fileName, int maxIteration, int attempt, short isPersist);

// directory in which to store output files
std::string output_directory;
// process rank
int rank;
// number of processes
int size;

// MPI error handler
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

// compute force of particle w.r.t. all bodies interacting with it
void computeForce(int particle, int particleCount, double** position, double* mass, double** force) {

	double directionX = 0.f;
	double directionY = 0.f;
	double magnitude = 0.f;
	double forceX = 0.f;
	double forceY = 0.f;
	
	int particleIndex = particle - (rank * particleCount/size);
	for (int j = 0; j < particleCount; j++) {
		if (j == particle) continue;
		directionX = position[j][ X ] - position[particle][ X ];
		directionY = position[j][ Y ] - position[particle][ Y ];						
		magnitude = sqrt((directionX * directionX) + (directionY * directionY));
		// Limit distance term to avoid singularities
		magnitude = std::max<float>( 0.5f * (mass[j] + mass[particle]), magnitude );

		
		forceX = (mass[j] * directionX) / (magnitude * magnitude * magnitude);
		forceY = (mass[j] * directionY) / (magnitude * magnitude * magnitude);	
		
		// we have to make sure to adjust index such that each particle writes to arrary 1st dim spaces 0..particleCount-1
		force[particleIndex][ X ] += forceX;
		force[particleIndex][ Y ] += forceY;
	}
}


// update velocity and position of body at timestep
void updateBody(int particle, int particlePerProcess, double** position, double** velocity, double** force) {
	
	int localIndex = particle - (rank * particlePerProcess);

	velocity[localIndex][ X ] += GTERM * force[localIndex][ X ] *  DELTA;
	velocity[localIndex][ Y ] += GTERM * force[localIndex][ Y ] *  DELTA;

	position[localIndex][ X ] += velocity[localIndex][ X ] * DELTA;
	position[localIndex][ Y ] += velocity[localIndex][ Y ] * DELTA;
}

// execute iterations to calculate future position of bodies in our set
void calculateSequential(forceFunction theForce, int particlePerProcess, int iterationCount, double** position, double** velocity, double* mass, short isPersist) {

	std::stringstream fileOutput;
	// we're sharing particles around MPI processes, each process starting off from particle: rank * particleCount 
	int startParticle = rank * particlePerProcess;	
	
	printf("Process [%d]: Calculating nbody %d to %d SEQUENTIAL over %d iterations\n", rank, startParticle, startParticle + particlePerProcess - 1, iterationCount);
	// we're not using symmetry to solve this problem.  Therefore each process only needs access to force array for own particles
	double** force = allocateMatrix(particlePerProcess, DIM);

	// point local position pointer to first particle processed by this process (Rank)
	double **localPosition = &position[rank * particlePerProcess];

	double* test;
	for (int timestep = 0; timestep < iterationCount; timestep++) {			

		test = &force[0][0];
		// set entire 2d array to 0;		
		test = (double*) memset(test, 0.f, particlePerProcess * DIM * sizeof(double));
		for (int i = startParticle; i < startParticle + particlePerProcess; i++) {
			theForce(i, particlePerProcess * size, position, mass, force);
		}

		// update particle positions and velocity
		for (int i = startParticle; i < startParticle + particlePerProcess; i++) {
			 updateBody(i, particlePerProcess, localPosition, velocity, force);
		}
				
		// we need to ensure that all processes receive updated positions such that forces can be correctly communicated at the next step
		MPI_Allgather(&localPosition[0][0], particlePerProcess * DIM, MPI_DOUBLE, &position[0][0], particlePerProcess * DIM, MPI_DOUBLE, MPI_COMM_WORLD);
	
		if (rank == 0 && isPersist) {			
			fileOutput.str(std::string());
			fileOutput << "nbody_" << timestep << ".txt";
			persistPositions(fileOutput.str(), output_directory, particlePerProcess * size, position, mass);
		} 	
	}
	

	freeMatrix(force, particlePerProcess);
}

void calculateParallel(forceFunction theForce, int particlePerProcess, int iterationCount, double** position, double** velocity, double* mass, int threadCount, short isPersist) {

	// set number of threads
	omp_set_num_threads(threadCount);
	// declare file output stream in case of persistance
	std::stringstream fileOutput;
		
	// we're sharing particles around MPI processes, each process starting off from particle: rank * particleCount 
	int startParticle = rank * particlePerProcess;	
	printf("Process [%d]: Calculating nbody %d to %d PARALLEL over %d iterations using %d threads\n", rank, startParticle, startParticle + particlePerProcess - 1, iterationCount, threadCount);
	// store forces
	double** force = allocateMatrix(particlePerProcess, DIM);

	// point local position pointer to first particle processed by this process (Rank)
	double **localPosition = &position[rank * particlePerProcess];

	double* test;
	for (int timestep = 0; timestep < iterationCount; timestep++) {			
		test = &force[0][0];
		// set entire 2d array to 0;		
		test = (double*) memset(test, 0.f, particlePerProcess * DIM * sizeof(double));
		#pragma omp parallel for shared(position, mass, force) schedule(static, 4)
		for (int i = startParticle; i < startParticle + particlePerProcess; i++) {
			theForce(i, particlePerProcess * size, position, mass, force);
		}
		
		#pragma omp parallel for shared(localPosition, velocity, force) schedule(static, 4)
		for (int i = startParticle; i < startParticle + particlePerProcess; i++) {
			 updateBody(i, particlePerProcess, localPosition, velocity, force);
		}
		
		// communicate updated positions at +delta to all processes
		MPI_Allgather(&localPosition[0][0], particlePerProcess * DIM, MPI_DOUBLE, &position[0][0], particlePerProcess * DIM, MPI_DOUBLE, MPI_COMM_WORLD);
		
		if (rank == 0 && isPersist) {			
			fileOutput.str(std::string());
			fileOutput << "nbody_" << timestep << ".txt";
			persistPositions(fileOutput.str(), output_directory, particlePerProcess * size, position, mass);
		} 	
								
	}					


	freeMatrix(force, particlePerProcess);		
}


// returns time to execute one simulation
double performSimulation(std::string fileName, int maxIteration, int attempt, short threadCount, short isPersist) {

	// our IVP body count
	int particleCount = 0;
	double runtime = 0.f;
	
	// allow master node to read input file and determine particle count...
	if (rank == 0) determineParticleCount(fileName, particleCount);
	
	// which should be shared with all other nodes
	MPI_Bcast(&particleCount, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// initialise position, velocity and mass arrays		
	double** position = allocateMatrix(particleCount, DIM);
	double** velocity = allocateMatrix(particleCount, DIM);
	double* mass = (double*)malloc(particleCount * sizeof(double));

	// assume that bodies is perfect multiple of MPI cluster size (4)
	int particlesPerProcess = particleCount / size;
	if (rank == 0) {
	// make first pass on input file to determine body count after which arrays can be initialised
		printf("Performing simulation over %d processes\n", size);
		printf("Particle count = %d; particles per process = %d\n", particleCount, particlesPerProcess);
		
		// initialise bodies based on input file
		initialise(fileName, position, velocity, mass);
	}

	// broadcast mass array to all processes since it's needed by all particles to compute force
	MPI_Bcast(mass, particleCount, MPI_DOUBLE, 0, MPI_COMM_WORLD); 	
	// broadcast position array to all processes
	MPI_Bcast(&position[0][0], particleCount * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// scatter velocity since velocity of particle p is not required within the context of particle q 
	double** localVelocity = allocateMatrix(particlesPerProcess, DIM);
 	MPI_Scatter(&velocity[0][0], particlesPerProcess * DIM, MPI_DOUBLE, &localVelocity[0][0], particlesPerProcess * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			
	// set computation strategy
	forceFunction func = &computeForce;
	// start clock	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	if (threadCount == 1) // run in distributed memory mode
		calculateSequential(func, particlesPerProcess, maxIteration, position, localVelocity, mass, isPersist);				
	else // run in hybrid mode
		calculateParallel(func, particlesPerProcess, maxIteration, position, localVelocity, mass, threadCount, isPersist);	
	
	// stop clock
	std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
	
	// base timing on master process
	if (rank == 0) {
		runtime = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())/1000000.0;
		printf("Attempt %d: Time elapsed serial: %f\n", attempt, runtime);	
		printf("--------------------------------------------------------------\n");
	} else runtime = 0.f;
	
		
	// cleanup memory
	free(mass);
	freeMatrix(localVelocity, particlesPerProcess);
	freeMatrix(position, particleCount);
	freeMatrix(velocity, particleCount);
	
	return runtime;
}



int main(int argc, char **argv) {	
	int errorCode;
	
	// max number of iterations specified by user
	int maxIteration = 0;
	int attempts = 0;
	short threadCount = 0;
	short persist = 0;
	// timing variable 
	double totalRuntime = 0.f;

	// initialize MPI environment
	MPI_Init (&argc, &argv);
	// get process id within MPI_COMM_WORLD
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	// get size of cluster
	MPI_Comm_size (MPI_COMM_WORLD, &size);   
	// setup error handling
	MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);


	/* Parameters:
	 * argv[1] = iterations
	 * argv[2] = input_file
	 * argv[3] = thread_count
	 * argv[4] = attempts
	 * argv[5] = do we persist?
	*/
	if (argc < 6) {
		if (rank == 0) {
			std::cerr << "Usage: mpiexec -np [node_count] nbody_mpi iteration_count input_file threads attempts persist[1|0]" << std::endl;
		}
    	MPI_Finalize();  
    	exit(0);
  	}
  	
  	maxIteration = atoi(argv[1]);
  	threadCount = atoi(argv[3]);
  	attempts = atoi(argv[4]);
	persist = atoi(argv[5]);
	output_directory = persist == 1 ? argv[6] : "foo";
	
	
	// get maxIteration from argument parameter
	for (int i = 0 ; i < attempts; i++) {
		totalRuntime += performSimulation(argv[2], maxIteration, i, threadCount, persist);		
	}

	if (rank == 0) 	printf("Average over %d attempts: %f\n", attempts, totalRuntime/ (double) attempts);
	
	MPI_Finalize();  		
	return 0;
}

