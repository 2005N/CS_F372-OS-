#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

void *runner(void* param){
    intptr_t i=(intptr_t)param;
    for(int j=0;j<i;j++){
        printf("* ");
    }
    printf("\n");
    pthread_exit(0);
}

int main(int argc,char *argv[]){
    int n = atoi(argv[1]);
    pthread_t tid[n];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for(int i=0;i<n;i++){
        pthread_create(&tid[i],&attr,runner,(void*)((intptr_t)i+1));
    }
    for (int i=0; i < n; i++)
	    pthread_join(tid[i],NULL);
}