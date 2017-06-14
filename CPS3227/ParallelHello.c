#include <stdio.h>
#include "omp.h"



int main () {

	// execute my hello world in parallel
	int id = 1;
	int threads = 10;

	omp_set_num_threads(5000);
	#pragma omp parallel private(id), shared(threads) 
	{
		//int id = omp_get_thread_num();
		threads = omp_get_num_threads();
		id = omp_get_thread_num();
//		printf("Hello World(%d, %d)\n", id, global_id);
		printf("Hello from thread %d out of threads %d\n", id, threads);

	}	
	// threads should wait for this joining point
	printf("%s\n", "Done!");
}
