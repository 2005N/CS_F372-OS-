#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 6

int main(int argc,char** argv){
    int pipe_fd[2];
    if(pipe(pipe_fd)==-1){
        perror("error in creating pipe");
        exit(1);
    }

    pid_t child_pid=fork();
    if(child_pid==-1){
        perror("error in creating child");
        exit(1);
    }else if(child_pid==0){
        close(pipe_fd[0]);
        int arr[]={1,2,3,4,5,6};
        write(pipe_fd[1],arr,sizeof(arr));
        close(pipe_fd[1]);
    }else{
        close(pipe_fd[1]);
        int parr[SIZE];
        read(pipe_fd[0],parr,sizeof(parr));
        int sum=0;
        for(int i=0;i<SIZE;i++) sum+=parr[i];
        printf("sum: %d\n",sum);
        close(pipe_fd[0]);
        wait(NULL);
    }
}