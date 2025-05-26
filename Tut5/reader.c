#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define SHM_KEY 0x1234
#define BUF_SIZE 8

int main(){
    int shmid;
    char* shmptr;
    char msg[BUF_SIZE]="";

    shmid=shmget(SHM_KEY,BUF_SIZE,0644 | IPC_CREAT);
    if(shmid==-1){
        perror("shared mem");
        return 1;
    }
    shmptr=shmat(shmid,NULL,0);
    if(shmptr==(void*)-1){
        perror("shared memory attach");
        return 1;
    }
    printf("waiting for messages from writer.press ctrl+c to exit.\n");

    while(1){
        if (strcmp(shmptr, msg) != 0) {
            printf("Received: %s\n", shmptr);
            strcpy(msg, shmptr);  // Update the previous message
        }

        // Add a short delay to prevent busy-waiting
        usleep(100000);  // 100 ms
    }

    if(shmdt(shmptr)==-1){
        perror("shmdt");
        return 1;
    }

    return 0;
}