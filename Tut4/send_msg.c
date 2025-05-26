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
    if((msgid=msgget(key,PERMS | IPC_CREAT))==-1){
        perror("error in sending");
        exit(1);
    }
    buf.mtype=1;
    strcpy(buf.mtext,"message1");
    if(msgsnd(msgid,&buf,strlen(buf.mtext)+1,0)==-1){
        perror("error in msgsnd");
        exit(1);
    }

    buf.mtype=2;
    strcpy(buf.mtext,"message2");
    if(msgsnd(msgid,&buf,strlen(buf.mtext)+1,0)==-1){
        perror("error in msgsnd");
        exit(1);
    }

    buf.mtype=3;
    strcpy(buf.mtext,"message3");
    if(msgsnd(msgid,&buf,strlen(buf.mtext)+1,0)==-1){
        perror("error in msgsnd");
        exit(1);
    }

    printf("messages sent successfully\n");

    return 0;
}