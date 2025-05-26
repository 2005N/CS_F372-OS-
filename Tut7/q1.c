#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //for sleep

#define N 5

int ind=0;
int arr[N];
pthread_mutex_t lock;

void *modify_array(void *arg){
    pthread_mutex_lock(&lock);

    ind++;
    usleep(100);
    arr[ind-1]=ind;

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(){
    pthread_t threads[N];

    if(pthread_mutex_init(&lock,NULL)!=0){
        printf("mutex initialization failed");
        return 1;
    }

    for(int i=0;i<N;i++){
        pthread_create(&threads[i],NULL,modify_array,NULL);
    }

    for(int i=0;i<N;i++){
        pthread_join(threads[i],NULL);
    }

    for(int i=0;i<N;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
    pthread_mutex_destroy(&lock);

    return 0;
}

//Without mutexes, multiple threads access shared data (ind and arr)
// concurrently, leading to race conditions, where the order of execution
// is unpredictable and the data may be corrupted.