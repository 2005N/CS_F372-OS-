#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum=0;
pthread_mutex_t lock;
void *solve(void* p){
    int val=*(int*)p;
    pthread_mutex_lock(&lock);
    sum+=val;
    printf("value: %d\n", sum);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    pthread_t t[4];
    if(pthread_mutex_init(&lock,NULL)!=0){
        perror("pthread");
        exit(1);
    }
    //int i=1,j=2;
    
    for(int i=0;i<4;i++){
        int *x=malloc(sizeof(int));
        *x =i+1;
        pthread_create(&t[i],NULL,solve,x);
    }
    
    //pthread_create(&t2,NULL,solve,&j);
    for(int i=0;i<4;i++){
        pthread_join(t[i],NULL);
    }
    
    //pthread_join(t2,NULL);

    if(pthread_mutex_destroy(&lock) != 0){
        perror("destroy mutex");
        exit(1);
    }

    return 0;
}