#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define PERMS 0644

struct myBuf{
    long mtype;
    char mtext[200];
};

int main(){
    struct myBuf buf;
    int msgid;
    int len;
    key_t key;
    if((key=ftok("w.c",'N'))==-1){
        printf("error in ftok");
        exit(1);
    }
    if((msgid=msgget(key,PERMS|IPC_CREAT))==-1){
        printf("error in msgget");
        exit(1);
    }

    while(1){
        if((msgrcv(msgid,&buf,sizeof(char)*200,0,0))==-1){
            printf("error in receiving msg");
            exit(1);
        }
        
        printf("Reader: %s \n",buf.mtext);
        if(strcmp(buf.mtext,"exit")==0) break;
    }
    if((msgrcv(msgid,&buf,sizeof(char)*200,0,0))==-1){
        printf("error in receiving msg");
        exit(1);
    }
    
    printf("Reader: %s \n",buf.mtext);
    if(msgctl(msgid,IPC_RMID,NULL)==-1){
        printf("error in deleting");
        exit(1);
    }

    return 0;
}