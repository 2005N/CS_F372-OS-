#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_CHILDREN 5  // Number of child processes
#define ARRAY_SIZE 2    // Size of the shared array

int sharedResource[ARRAY_SIZE]={0,0};
sem_t sem[ARRAY_SIZE];

void *childProcess(void* arg){
    int id=*((int*)arg);
    int index=id%ARRAY_SIZE; //i%2;
    while(1){
        sem_wait(&sem[index]);
        sharedResource[index]++;
        printf("child %d sharedResource[%d]: %d\n",id,index,sharedResource[index]);
        sleep(2);
        sem_post(&sem[index]);
        sleep(1);
    }
    return NULL;
}

int main(){
    pthread_t children[NUM_CHILDREN];
    int ids[NUM_CHILDREN]; // to store process id's

    for(int i=0;i<ARRAY_SIZE;i++){
        sem_init(&sem[i],0,1); //only one process can access each element at a time
    }

    for(int i=0;i<NUM_CHILDREN;i++){
        ids[i]=i;
        pthread_create(&children[i],NULL,childProcess,(void*)&ids[i]);
    }

    for(int i=0;i<NUM_CHILDREN;i++){
        pthread_join(children[i],NULL);
    }

    for(int i=0;i<ARRAY_SIZE;i++){
        sem_destroy(&sem[i]);
    }

    return 0;
}