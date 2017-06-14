//----------------------------------------------------------------------------------------------
//	Filename:	nbody.cpp
//	Author:		Keith Bugeja
//----------------------------------------------------------------------------------------------
//  CPS3227 assignment for academic year 2017/2018:
//	Sample naive [O(n^2)] implementation for the N-Body problem.
//----------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include "omp.h"

#include "vector2.h"

/*
 * Constant definitions for field dimensions, and particle masses
 */

const int fieldWidth = 1000;
const int fieldHalfWidth = fieldWidth >> 1;
const int fieldHeight = 1000;
const int fieldHalfHeight = fieldHeight >> 1;

const double minBodyMass = 2.5f;
const double maxBodyMassVariance = 5.f;

/*
 * Particle structure
 */
struct Particle
{
	Vector2 Position;
	Vector2 Velocity;
	double	Mass;
	
	Particle(void) 
		: Position( ((double)rand()) / RAND_MAX * fieldWidth - fieldHalfWidth,
					((double)rand()) / RAND_MAX * fieldHeight - fieldHalfHeight)
		, Velocity( 0.f, 0.f )
		, Mass ( ((double)rand()) / RAND_MAX * maxBodyMassVariance + minBodyMass )
	{ }
	
	Particle(double p_Mass, double p_X, double p_Y)
		: Position(p_X, p_Y)
		, Velocity (0.f, 0.f)
		, Mass (p_Mass)
	{ }
};

/*
 * Compute forces of particles exerted on one another
 */
void ComputeForces(std::vector<Particle> &p_bodies, double p_gravitationalTerm, double p_deltaT)
{
	Vector2 direction,
		force, acceleration;

	double distance;

	for (size_t j = 0; j < p_bodies.size(); ++j)
	{
		Particle &p1 = p_bodies[j];
	
		force = 0.f, acceleration = 0.f;
	
		for (size_t k = 0; k < p_bodies.size(); ++k)
		{
			if (k == j) continue;
		
			Particle &p2 = p_bodies[k];
			
			// Compute direction vector
			direction = p2.Position - p1.Position;
			
			// Limit distance term to avoid singularities
			distance = std::max<double>( 0.5f * (p2.Mass + p1.Mass), direction.Length() );
			
			// Accumulate force
			force += direction / (distance * distance * distance) * p2.Mass; 
		}
				
		// Compute acceleration for body 
		acceleration = force * p_gravitationalTerm;
		
		// Integrate velocity (m/s)
		p1.Velocity += acceleration * p_deltaT;
	}
}

/*
 * Update particle positions
 */
void MoveBodies(std::vector<Particle> &p_bodies, double p_deltaT)
{
	for (size_t j = 0; j < p_bodies.size(); ++j)
	{
		p_bodies[j].Position += p_bodies[j].Velocity * p_deltaT;
	
	}
}

/*
 * Commit particle masses and positions to file in CSV format
 */
void PersistPositions(const std::string &p_strFilename, std::vector<Particle> &p_bodies)
{
	std::cout << "Writing to file: " << p_strFilename << std::endl;
	
	std::ofstream output(p_strFilename.c_str());
	
	if (output.is_open())
	{	
		for (int j = 0; j < p_bodies.size(); j++)
		{
			output << 	p_bodies[j].Mass << ", " <<
				p_bodies[j].Position.Element[0] << ", " <<
				p_bodies[j].Position.Element[1] << std::endl;
		}
		
		output.close();
	}
	else
		std::cerr << "Unable to persist data to file:" << p_strFilename << std::endl;

}


/*
 * Load particles from input file to vector reference
 */
void LoadParticlesToVector(const std::string &p_inputFilename, std::vector<Particle> &p_bodies) {
	std::cout << "Loading particles from file: " << p_inputFilename << std::endl;
	std::ifstream input(p_inputFilename.c_str());	
  	std::string line;

	double *dblTokens;
	dblTokens = new double[3]; // same as malloc(3 * sizeof(double))
	int counter;
  	if (input.is_open())
  	{
    	while (getline(input, line))
    	{
			// remove space from line
			line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
			// parse line into tokens
			std::istringstream tokens(line);
			std::string token;
			counter = 0;
			while (getline(tokens, token, ','))
			{			
      			// convert string token to double value and store in array
      			dblTokens[counter++] = atof(token.c_str());
    		}
    		// create new particle and push to vector
    		p_bodies.push_back(Particle(dblTokens[0], dblTokens[1], dblTokens[2]));
    	}
    	input.close();
 	}
 	else
		std::cerr << "Unable to read data to file:" << p_inputFilename << std::endl;
		
	delete(dblTokens);
}


int main(int argc, char **argv)
{
	int particleCount = 0;
	int maxIteration = 0;
	const double deltaT = 0.01f;
	const double gTerm = 20.f;
	double runtime;

	 if (argc < 3) {
    	std::cerr << "Usage: nbody_seq_file iteration_count input_file" << std::endl;
    	exit(1);
  	}

	// get maxIteration from argument parameter
  	maxIteration = atoi(argv[1]);

	std::stringstream fileOutput;
	std::vector<Particle> bodies;

	LoadParticlesToVector(argv[2], bodies);
	particleCount = bodies.size();
	
	std::cout << "Particle count = " << particleCount << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
/*	for (int bodyIndex = 0; bodyIndex < particleCount; ++bodyIndex)
		bodies.push_back(Particle());*/
	runtime = omp_get_wtime();			
	for (int iteration = 0; iteration < maxIteration; ++iteration)
	{
		ComputeForces(bodies, gTerm, deltaT);
		MoveBodies(bodies, deltaT);
		
//		fileOutput.str(std::string());
//		fileOutput << "nbody_" << iteration << ".txt";
//		PersistPositions(fileOutput.str(), bodies);
	}
	runtime = omp_get_wtime() - runtime;
	std::cout << "Time elapsed = " << runtime << std::endl;

	return 0;
}
