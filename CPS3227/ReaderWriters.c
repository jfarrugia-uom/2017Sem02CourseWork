#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "omp.h"


int main() {
	int readers = 4;
	int writers = 3;
	omp_lock_t lock;
	omp_init_lock(&lock);
	

	// simulating readers here
	omp_set_num_threads(readers);
	int me = 0;
	printf("Reading starts...\n");
	#pragma omp parallel private(me)
	{
		me = omp_get_thread_num();
		printf("Thread [%d] Entering shared memory\n", me);
		sleep(2);
		printf("Thread [%d] Reading shared memory\n", me);
		sleep(1);
		printf("Thread [%d] Leaving shared memory\n", me);
		sleep(1);
	}

	printf("Writing starts...\n");
	// simulating writers here
	omp_set_num_threads(writers);
	#pragma omp parallel private(me)
	{
		me = omp_get_thread_num();
		//omp_set_lock(&lock);
		#pragma omp critical
		{
			printf("Thread [%d] Entering shared memory\n", me);
			sleep(2);
			printf("Thread [%d] Writing/Modifying shared memory\n", me);
			sleep(1);
			printf("Thread [%d] Leaving shared memory\n", me);
			sleep(1);	
		}
		//omp_unset_lock(&lock);
	}		
	//omp_destroy_lock(&lock);
	printf("\n");
}
