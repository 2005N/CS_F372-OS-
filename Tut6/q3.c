#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

typedef struct{
    int x,y,z;
}Point;

Point arr[3];
int dist[3];
void *runner(void* param){
    intptr_t i=(intptr_t)param;
    int x=arr[i].x;
    int y=arr[i].y;
    int z=arr[i].z;

    dist[i]=x*x + y*y + z*z;
    pthread_exit(0);
}

int main(){
    pthread_t tid[3];
    pthread_attr_t attr; 
	pthread_attr_init(&attr); 
    arr[0].x=1; arr[0].y=2; arr[0].z=3;
    arr[1].x=4; arr[1].y=5; arr[1].z=6;
    arr[2].x=7; arr[2].y=8; arr[2].z=9;
    
    // Create 3 threads, each computing the square of the distance for one point
    for (int i = 0; i < 3; i++) {
        pthread_create(&tid[i], &attr, runner, (void*)((intptr_t)i));
    }

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    // Output the squared distances
    for (int i = 0; i < 3; i++) {
        printf("Squared distance of point %d: %d\n", i + 1, dist[i]);
    }

    return 0;
}