#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int sum=0;
sem_t sem;

void *solve(void *p){
    int val = *(int*)p;
    sem_wait(&sem); //wait for semaphore
    //start of critical section
    sum+=val;
    printf("value: %d\n",sum);
    //end of critical section
    sem_post(&sem); // signal the semaphore
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    pthread_t t1,t2;

    sem_init(&sem,0,1); //initial value of semaphore=1
    int i=1,j=2;
    pthread_create(&t1,NULL,solve,&i);
    pthread_create(&t2,NULL,solve,&j);

    pthread_join(t1,NULL); pthread_join(t2,NULL);

    sem_destroy(&sem);

    return 0;
}