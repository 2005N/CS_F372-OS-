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
    if((msgid=msgget(key,PERMS))==-1){
        printf("error in msgget");
        exit(1);
    }
    buf.mtype=1;

    while(1){
        printf("enter message: ");
        fgets(buf.mtext, sizeof(buf.mtext), stdin); // Get user input and store it in buf.mtext

        // Remove newline character from input
        buf.mtext[strcspn(buf.mtext, "\n")] = '\0';

        len=strlen(buf.mtext);
        if((msgsnd(msgid,&buf,len+1,0))==-1){
            printf("error in sending msg");
            exit(1);
        }
        
        if(strcmp(buf.mtext,"exit")==0) break;
    }
    

    return 0;
}