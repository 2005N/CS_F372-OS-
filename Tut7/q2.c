#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

typedef struct {
    pthread_mutex_t mutex;
    int index;
} shared_data_t;

int main(){
    shared_data_t *shared_data;
    shared_data->index=0;
    for(int i=0;i<5;i++){
        pid_t pid=fork();
        if(pid<0){
            perror("fork");
            exit(1);
        }
        if(pid==0){
            pthread_mutex_lock(&shared_data->mutex);
            printf("child %d: index = %d\n",i+1,shared_data->index);
            shared_data->index+=1;
            pthread_mutex_unlock(&shared_data->mutex);

            //exit the child process
            exit(0);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    return 0;
}