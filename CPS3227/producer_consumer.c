#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "omp.h"


int fetchAndAdd(int* variable, int increment);

int* allocateArray(int elements) {
	int* x;
	x = malloc(elements * sizeof (int));
	return x;
}

void populateArray(int* x, int elements) {
	srand ( time(NULL) );
	for (int i = 0; i < elements; i++) {		
		x[i] = rand() % 100;
		printf("x[%d] = %d\n", i, x[i]);
	//	sleep(1);
	}
}

void printArray(int* x, int elements) {
	for (int i = 0; i < elements; i++) {
		printf("x[%d] = %d\n", i, x[i]);
	}
}
 

void computeVectorProduct() {
	int *x;
	int *y;
	int size = 100000;
	int total = 0;
		
	double runtime;

	x = allocateArray(size);
	y = allocateArray(size);

	printf("Populating array\n");
	populateArray(x, size);
	populateArray(y, size);

	printf("Performing operation  sequentially\n");	
	runtime = omp_get_wtime();
	for (int i = 0; i < size; i++) {
		total +=  x[i] * y[i];
	}
	runtime = omp_get_wtime() - runtime;
	printf("Total = %d in %f\n", total, runtime);


	omp_set_num_threads(2);
	printf("Performing operation with atomic critical section\n");	
	int temp = 0;
	total = 0;

	runtime = omp_get_wtime();
	#pragma omp parallel for private(temp) shared(total)
	for (int i = 0; i < size; i++) {
		temp = x[i] * y[i];
//		printf("Thread [%d] computing i = %d temp = %d \n", omp_get_thread_num(), i, temp);
		#pragma omp atomic
		total += temp;
	}	
	runtime = omp_get_wtime() - runtime;
	printf("Total = %d in %f\n", total, runtime);

	total = 0;
	printf("Performing operation with reduction method\n");
//	start = clock();
	runtime = omp_get_wtime();
	#pragma omp parallel for private(temp) reduction(+:total)
	for (int i = 0; i < size; i++) {
		temp = x[i] * y[i];
//		printf("Thread [%d] computing i = %d temp = %d \n", omp_get_thread_num(), i, temp);
		total += temp;
	}
//	end = clock();
	runtime = omp_get_wtime() - runtime;
	printf("Total = %d in %f\n", total, runtime);
}

void testSchedule() {
	omp_set_num_threads(2);
	#pragma omp parallel for schedule(static, 10) 
	for (int i = 0; i < 100; i++) {
		printf("Thread [%d], doing %d\n", omp_get_thread_num(), i);
	}
}

void test() {

	omp_set_num_threads(2);

	#pragma omp parallel 
//	printf("main: Thread [%d]\n", id);	
	#pragma omp sections  
	{
		#pragma omp section 
		{
			for(int i = 0; i < 5; i++) {
				int id =  omp_get_thread_num();
				printf("a: Thread [%d]\n",id);
				sleep(1);
			}
		}
		
		#pragma omp section
		{
			for (int i = 0; i < 5; i++) {
				int id = omp_get_thread_num();
				printf("b: Thread [%d]\n",id);
				sleep(1);
			}				

		}
	}
}

void test2() {
	int max = 11;
	int a[2] = {22, 33};
	int tid = 0;
	
	omp_set_num_threads(2);
	#pragma omp parallel 
	{
		#pragma omp critical
		{
			tid = omp_get_thread_num();
			printf("a[tid] = %d\n", a[tid]);
			if (a[tid] > max) { 
				max = a[tid];
			}
		}		
	}
	printf("max = %d\n", max);	
}

void test3() {
	int* a;
	a = allocateArray(4);
	omp_set_num_threads(2);
	#pragma omp parallel
	{
		#pragma omp for 
		for (int i = 0; i < 4; i++) {
			a[i] = i * i;
		}
		
		#pragma omp master
		for (int i = 0; i < 4; i++) {
			printf("Thread[%d]: Partial result a[%d] = %d\n", omp_get_thread_num(), i, a[i]);
		}
		#pragma omp barrier
		
		#pragma omp for
		for (int i = 0; i < 4; i++) {
			a[i] += i;
		}
	}
	
	for (int i = 0; i < 4; i++) {
			printf("Final result a[%d] = %d\n", i, a[i]);
		}	
}

void test4() {
	omp_set_num_threads(2);
	int id;
	#pragma omp parallel private(id) 
	{
		#pragma omp for nowait
		for (int i = 0 ; i < 10; i++) {
			id = omp_get_thread_num();
			printf("A) Thread [%d] running\n", id);
		}
	
		#pragma omp for nowait
		for (int i = 0 ; i < 10; i++) {
			id = omp_get_thread_num();
			printf("B) Thread [%d] running\n", id);

		}
	}	
}


int sumArray(int* array, int elements) {
	int sum = 0;
	for (int i = 0; i < elements; i++) {
		sum += array[i];	
	}
	return sum;
}

void producerConsumer1() {
//	omp_set_num_threads(2);
	int flag = 0;
	int sum;	
	int *buffer;
	buffer = allocateArray(10);
	int tid;
	#pragma omp parallel sections num_threads(2) private(tid)
	{
 		#pragma omp section
 		{
 			tid = omp_get_thread_num();
 			while (1) {
	 			if (flag == 0) {
		 			printf("Thread[%d]: Populate array...\n", tid);			
		 			populateArray(buffer, 10);
		 			#pragma omp flush
		 			flag = 1;
		 			#pragma omp flush(flag)
	 			}
	 			sleep(1);
	 		}
 		}
 		#pragma omp section
 		{
			tid = omp_get_thread_num();
			while (1) {				
				#pragma omp flush (flag)
				if (!flag) {
				 	#pragma omp flush(flag)
				}				 
				#pragma omp flush				 
				if (flag == 1) {
					sum = sumArray(buffer, 10);
					printf("Thread[%d]: Sum = %d\n", tid, sum);
					flag = 0;
					#pragma omp flush(flag)		
				}
				sleep(1);
			}
		}
	}	
}


void producerConsumerWithCritical() {
	omp_set_num_threads(4);
	int *buffer;
	int sharedCounter = 0;	
	int BUFFER_SIZE = 10;
	buffer = allocateArray(BUFFER_SIZE);
	int tid;
	int privateCounter = 0;
	#pragma omp parallel private(tid) firstprivate(privateCounter) shared(sharedCounter)
	{
 		while (1) {
	 		tid = omp_get_thread_num();
	 		// ensures only master thread acts as producer
	 		#pragma omp master
	 		{	 			
	 			printf("Thread [%d]: Populate array...\n", tid);			
	 			populateArray(buffer, BUFFER_SIZE);
	 			// reset counter
	 			sharedCounter = 0;
				printf("counter = %d\n", sharedCounter);
	 			sleep(1);
	 		}
	 		// don't allow threads beyond this until master completes production
	 		#pragma omp barrier	 			 			 		
//			while(sharedCounter < BUFFER_SIZE) 
			while(privateCounter < BUFFER_SIZE)
			{
				if (privateCounter+1 >= BUFFER_SIZE) {
					continue;
				} else {
					privateCounter = fetchAndAdd(&sharedCounter, 1);
					printf("Thread [%d]: Consume buffer %d private counter = %d shared counter = %d \n", tid, buffer[privateCounter], privateCounter, sharedCounter);
				}
				
				/*tid = omp_get_thread_num();
				#pragma omp critical 
				{
					// in case two threads enter the while loop at the same time make sure that sharedCount is still smaller than BUFFER_SIZE
					if (sharedCounter < BUFFER_SIZE) {
						printf("Thread [%d]: Consume buffer %d\n", tid, buffer[sharedCounter]);
						sharedCounter++;								
					}
				}*/
				sleep(1);			
			}										
		}
	}	
}

void producerConsumerWithFlush() {
	omp_set_num_threads(2);
	int sharedCounter = 10;
	int flag = 0;	
	int *buffer;
	int BUFFER_LIMIT = 10;
	buffer = allocateArray(BUFFER_LIMIT);
	int tid;
	#pragma omp parallel sections private(tid) shared(sharedCounter)
	{
 		#pragma omp section
 		{
 			tid = omp_get_thread_num();
 			while (1) {
	 			if (flag == 0) {
		 			printf("Thread[%d]: Populate array...\n", tid);			
		 			populateArray(buffer, BUFFER_LIMIT);
		 			sharedCounter = 0;
		 			#pragma omp flush		 			
		 			flag = 1;
		 			#pragma omp flush(flag)
	 			}
	 			printf("Producer Thread [%d]: %d\n", tid, flag);
	 			sleep(1);
	 		}
 		}
 		#pragma omp section
 		{
			tid = omp_get_thread_num();
			while (1) {				
				#pragma omp flush (flag)
				if (!flag) {
				 	printf("Consumer Thread [%d] is in here\n", tid);
				 	#pragma omp flush(flag)
				 	sleep(1);
				}				 
				#pragma omp flush				 
				if (flag == 1) {					
					#pragma omp critical 
					{
						printf("Thread[%d]: Consume buffer = %d\n", tid, buffer[sharedCounter]);
						sharedCounter++;					
					}
					if (sharedCounter == BUFFER_LIMIT) {				
						flag = 0;
						#pragma omp flush(flag)
					}								
				}				
			}
		}
	}	
}

void lockExample() {
	omp_set_num_threads(2);
	int tid;
	int in1, in2;
	in1 = in2 = 0;
//	int *buffer;
//	int BUFFER_LIMIT = 10;
//	buffer = allocateArray(BUFFER_LIMIT);

	#pragma omp parallel sections private(tid) shared(in1, in2) 
	{
 		#pragma omp section
 		{
 			tid = omp_get_thread_num();
 			while (1) {
				if (in2 == 0) {
					#pragma omp atomic write
					in1 = 1;
				} else {
					#pragma omp flush
					continue;
				}
				printf("Thread [%d] In1: Do some interesting stuff\n", tid);
				#pragma omp atomic write
				in1 = 0;
				#pragma omp flush(in1)
				sleep(1);											
	 		}
 		}
 		#pragma omp section
 		{
			tid = omp_get_thread_num();
			while (1) {				
				if (in1 == 0) {				
					#pragma omp atomic write
					in2 = 1;
				} else {			
					#pragma omp flush
					continue;
				}				
				printf("Thread [%d] In2: Do some interesting stuff\n", tid);				

				#pragma omp atomic write
				in2 = 0;
				#pragma omp flush(in2)
				sleep(1);
			}
		}
	}	
}

int fetchAndAdd(int* variable, int increment) {
	int temp;
	#pragma omp atomic capture
	{ temp = *variable; *variable += increment; }
	return temp;
}


void producerConsumerAtomic(){
	omp_set_num_threads(4);
	int counter = 0;
	int _counter = 0;
	int* buffer;
	int limit = 10;
	buffer = allocateArray(limit);
	#pragma omp parallel private(_counter) shared(counter)
	{
		_counter = 0;
		while (1) {
			#pragma omp single
			{
				printf("Thread[%d] Producer\n", omp_get_thread_num());
				populateArray(buffer, limit);
				counter = 0;
				printf("------------------------------\n");
			}		
			#pragma omp barrier
			//printf("Thread [%d] - Working\n", omp_get_thread_num());		
			_counter = 0;
			while(_counter < limit) {							
				_counter = fetchAndAdd(&counter, 1);
				if (_counter >= limit) {
					continue;
				} else {
					printf("Thread [%d] counter = %d value = %d\n", omp_get_thread_num(), _counter, buffer[_counter]);					
					usleep(500000);
					//sleep(1);
				}
			}			
		}
	}
}


int testAndSet(int* lock) {
	int temp;
	#pragma omp atomic capture
	{temp = *lock; *lock = 1; }
	return temp;	
}

void spinLockTest() {
	omp_set_num_threads(2);
	int lock = 0;
	int sharedCounter1 = 0;
	int sharedCounter2 = 0;
	int privateCounter = 0;
	#pragma omp parallel sections shared(lock, sharedCounter1, sharedCounter2) private(privateCounter)
	{

		#pragma omp section 
		{
			privateCounter = 0;
			while(1) {
				while (testAndSet(&lock)) { printf("A awaiting...\n"); sleep(1); }
				//#pragma omp critical (x)
				//{
					printf("A - In critical section\n");
				//}
				sharedCounter1 += sharedCounter2 + 5;
				lock = 0;
				privateCounter++;
			}			
		}		
		#pragma omp section 
		{
			privateCounter = 0;
			while(1) {
				while (testAndSet(&lock)) { printf("B awaiting...\n"); sleep(1); }
				//#pragma omp critical (x)
				//{
					printf("B - In critical section\n");
				//}
				sharedCounter2 += sharedCounter1 + 2;
				lock = 0;
				privateCounter++;

			}
		}
	}
	printf("counter = %d, %d\n", sharedCounter1, sharedCounter2);
}

int main() {
	
/*	int x[4] = {1,2,3,4};
	int y[4] = {3,4,5,6};*/
//	computeVectorProduct();	
//	producerConsumerWithCritical();
//	spinLockTest();	
	int*x = malloc(sizeof(int) * 5);
	populateArray(x, 5);	
	printf("-------------------\n");	
	int*y = realloc(x, 10 * sizeof(int));
	x = y;
	for (int i = 5; i < 10; i++) {
		*(x+i) = i;
	}
	printArray(x, 10);
			
}
