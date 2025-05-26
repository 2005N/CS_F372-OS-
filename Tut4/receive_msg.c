#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/types.h>

#define PERMS 0644

struct myBuf{
    long mtype;
    char mtext[200];
};

int main(){
    struct myBuf buf;
    int msgid;
    key_t key;
    if((key=ftok("send_msg.c",'A'))==-1){
        perror("error in ftok");
        exit(1);
    }
    if((msgid=msgget(key,PERMS))==-1){
        perror("error in sending");
        exit(1);
    }
    
    if(msgrcv(msgid,&buf,sizeof(buf.mtext),3,0)==-1){
        perror("error in msgrcv");
        exit(1);
    }
    printf("%s\n",buf.mtext);

    if(msgrcv(msgid,&buf,sizeof(buf.mtext),2,0)==-1){
        perror("error in msgrcv");
        exit(1);
    }
    printf("%s\n",buf.mtext);

    if(msgrcv(msgid,&buf,sizeof(buf.mtext),1,0)==-1){
        perror("error in msgrcv");
        exit(1);
    }
    printf("%s\n",buf.mtext);

    if(msgctl(msgid,IPC_RMID,NULL)==-1){
        perror("error in deleting queue");
        exit(1);
    }

    return 0;
}