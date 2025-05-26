#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

int sum = 0;
sem_t sem;

void *solve(void *p) {
	int val = *(int *)p;
	sem_wait(&sem); // Wait for the semaphore
	// Start of Critical Section
	sum += val;
	printf("Value: %d Sum: %d\n", val, sum);
	// End of Critical Section
	sem_post(&sem); // Signal the semaphore
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	pthread_t thread1, thread2;

	//sem = sem_open("SEM", O_CREAT, 0666, 1);
	// Initialize Semaphore with an initial value of 1
	sem_init(&sem, 0, 1);

	int i = 1, j = 2;
	pthread_create(&thread1, NULL, solve, &i);
	pthread_create(&thread2, NULL, solve, &j);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	// Destroy Semaphore
	sem_destroy(&sem);

	return 0;
}
