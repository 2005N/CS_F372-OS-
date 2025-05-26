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
    printf("Enter strings (newline terminated) to send to reader. Press Ctrl+C to exit.\n");

    while(1){
        if(fgets(shmptr,BUF_SIZE,stdin)==NULL){
            perror("fgets");
            return 1;
        }
        shmptr[strcspn(shmptr,"\n")]='\0';
    }

    if(shmdt(shmptr)==-1){
        perror("shmdt");
        return 1;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        return 1;
    }

    return 0;
}