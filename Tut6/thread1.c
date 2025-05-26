#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

void *printID(void *arg){
    printf("POSIX thread id: %ld\n",(long)pthread_self());
    printf("LINUX thread id: %ld\n",(long)gettid());
    pthread_exit(NULL);
}

int main(){
    pthread_t tid;
    int rc=pthread_create(&tid,NULL,printID,NULL);
    if(rc){
        printf("error in creating thread %d\n",rc);
        exit(-1);
    }
    pthread_join(tid,NULL);

    return 0;
}