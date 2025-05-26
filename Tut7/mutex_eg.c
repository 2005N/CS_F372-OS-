#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum = 0;
pthread_mutex_t lock;
void *solve (void *p)
{
	int val = *(int *)p;
	//pthread_mutex_lock (&lock);
	//Start of Critical Section
	sum += val;
	printf ("Value: %d Sum: %d\n", val, sum);
	//End of Critical Section
	//pthread_mutex_unlock (&lock);
	pthread_exit (NULL);
}

int main(int argc, char* argv[]) {
	pthread_t thread1, thread2;
	//Initialize Mutex
	if(pthread_mutex_init(&lock, NULL) != 0){
		printf("Error Initializing Mutex\n");
		return -1;
	}
	
	int i = 1, j = 2;
	pthread_create(&thread1, NULL, solve, &i);
	pthread_create(&thread2, NULL, solve, &j);
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	//Destroy Mutex
	if(pthread_mutex_destroy(&lock) != 0){
		printf("Error Destroying Mutex\n");
		return -1;
	}
	return 0;
}
