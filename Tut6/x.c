#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

#define NUM_THREADS 10
int sum; /* this data is shared by the thread(s) */ 
void *runner(void *param); /* threads call this function */

int main(int argc, char *argv[]) {
	sum = 0;
	pthread_t tid[NUM_THREADS]; /* the thread identifier */ 
	pthread_attr_t attr; /* set of thread attributes */
	/* set the default attributes of the thread */ 
	pthread_attr_init(&attr); 
	/* create the thread */ 
	for (int i=0; i < NUM_THREADS; i++)
		pthread_create(&tid[i], &attr, runner, (void*)((intptr_t)i+1)); 
	
	/* wait for the thread to exit */ 
	for (int i=0; i < NUM_THREADS; i++)
	pthread_join(tid[i],NULL);
    
	printf("sum = %d\n",sum); 
}

/* The thread will execute in this function */ 
void *runner(void *param) {
	intptr_t i = (intptr_t)param; 
	printf("Thread: %ld, sum: %d\n", i, sum);
	sum += i;
	pthread_exit(0);
}