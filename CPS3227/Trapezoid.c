#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "omp.h"

typedef double (*approxFunction)(double x);

double trapezoid(approxFunction theFunc, int N, double a, double b);
double myFunc(double x);


double myFunc(double x) {
	//double pi = acos(-1);
	// convert to radians
	//double radian = (pi/180) * x;	
	// slow down computation
	return (x*x) + (2.0f * sin(x));
}

double piFunc(double x) {
	return 4 / (1 + (x * x));
}

double trapezoid(approxFunction theFunc, int N, double a, double b) {
	int index = 0;
	double step = 0.0, result = 0.0;
	
	double currentStep = a;
	step = (b - a) / (double) N;
	printf("In trapezoid function; step size = %f; N = %f\n",  step, (double)N);
	for (index = 0; index < N; index++) {		
		result += theFunc(currentStep) + theFunc(currentStep + step);
//		printf("a = %0.8f; b = %0.8f\n", currentStep, currentStep + step);
		currentStep += step;
		
	}
	return result * (step * 0.5);
}


double fineTrap(approxFunction theFunc, int N, double a, double b) {
			
	int NUM_THREADS = 2;
	omp_set_num_threads(NUM_THREADS);

	double interval = ((double) b - (double) a) / (double) N;	
	double _a = 0.0;
	double result = 0.0;
	printf("interval  = %0.12f\n", interval);
	#pragma omp parallel for private(_a) reduction(+: result)
	for (int i = 0; i < N; i++) {
		_a = a + (interval * (double) i);
//		printf("a = %f; b = %f\n", _a, _a+interval);
		result += theFunc(_a) + theFunc(_a + interval);
	}
 	
	return result * interval * 0.5;
}

double coarseTrap(approxFunction theFunc, int N, double a, double b) {
	int NUM_THREADS = 10;
	omp_set_num_threads(NUM_THREADS);
	
	double final = 0.0;
	int jobSize = N / NUM_THREADS;
	double interval = ((double) b - (double) a) / (double) N;
	printf("split sample size = %d\n", jobSize);
	printf("interval  = %0.12f\n", interval);
	#pragma omp parallel
	{
		int id;
		double x = 0.0;
		
		id = omp_get_thread_num();
		
		double newA = a + ((double)id * (double) jobSize * (double) interval);		
		double newB = newA + (jobSize * interval);
		printf("Thread id (%d);newA = %f; newB = %f\n", id, newA, newB);
		x += trapezoid(theFunc, jobSize, newA, newB);
		#pragma omp critical
		final += x;		
	}	
	return final;
}


void simpleArrayAddition() {
	int simple[100000];
	for (int i = 0; i < 100000; i++) {
		simple[i] = i+1;
	}	
	
	long  partial = 0;
	int  nthreads; 
	omp_set_num_threads(2);
	
	#pragma omp parallel
	{
		int i, id, nthrds;
		long x;
		id = omp_get_thread_num();
		nthrds = omp_get_num_threads();
		
		// only allow 1 thread to update global variable
		if (id == 0)  nthreads = nthrds;   
		printf("id = %d; nthrds = %d; nthreads = %d\n", id, nthrds, nthreads);
		// this patterns is common in SPMD (single program multiple data) loops
		for ( i = id; i < 100000; i = i + nthrds) {
//			printf("In loop index %d: Thread %d of %d\n",  i, omp_get_thread_num(), omp_get_num_threads());
			// we store result in a private variable
			x += simple[i];
		}
		// make sure that only thread at a time attempts to update partial
		#pragma omp critical
		partial += x;		
			
	}
	printf("Answer = %ld\n", partial);
}

void doTrapezoidStuff() {
	approxFunction approx = NULL;
	approx = &myFunc;
				
	double result = trapezoid(approx, 1000000, 0.0, 10.0);
	printf("sequential result = %f\n", result);
	result = coarseTrap(approx, 1000000, 0.0, 10.0);
	printf("coarse grained parallel result = %f\n", result);
	result =   fineTrap(approx, 10000000, 0.0, 10.0);
	printf("fine grained parallel result = %f\n", result);

}


int main() {
	
/*	int y[2] = {1,2};
	passArrayByReference(&y[0], 2);*/
	// same as 	passArrayByReference(y, 2);	

	doTrapezoidStuff();

}

