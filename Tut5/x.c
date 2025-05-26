#include <stdio.h>
#include <stdlib.h>
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
    pid_t parent_pid = getpid();

    if(fork()==0){
        sleep(3);
        shmid=shmget(SHM_KEY,BUF_SIZE,0644);
        if(shmid==-1){
            perror("shared memory");
            return 1;
        }

        shmptr=shmat(shmid,NULL,0);
        if(shmptr==(void*)-1){
            perror("shared memory attach");
            return 1;
        }
        printf("child: %s\n",shmptr);
        // Convert the read string to pid_t for comparison
        pid_t read_pid = atoi(shmptr);
        if (read_pid == parent_pid) {
            printf("Child: PID matches parent's PID (%d)\n", read_pid);
        } else {
            printf("Child: PID does not match. Parent's PID: %d, Read PID: %d\n", parent_pid, read_pid);
        }
        if(shmdt(shmptr)==-1){
            perror("shmdt");
            return 1;
        }
    }else{
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
        sprintf(shmptr,"%d",parent_pid);
        printf("parent writing done\n");
        wait(NULL);

        if(shmdt(shmptr)==-1){
            perror("shmdt");
            return 1;
        }
        if(shmctl(shmid,IPC_RMID,0)==-1){
            perror("shmctl");
            return 1;
        }
    }
    return 0;
}