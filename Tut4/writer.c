#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define PERMS 0644

// Struct for the message
struct my_msgbuf {
long mtype;
char mtext[200];
};

int main() {
struct my_msgbuf buf;
int msqid;
int len;
key_t key;
// Generate System V IPC key
if ((key = ftok("writer.c", 'B')) == -1) {
printf("Error in ftok");
exit(1);
}
// Create the message queue and get its ID using key
if ((msqid = msgget(key, PERMS)) == -1) {
printf("Error in getting message queue ID. Did you run the reader?");
exit(1);
}
buf.mtype = 1;
sprintf(buf.mtext, "%s", "Hi");
len = strlen(buf.mtext);
// Send the message
if (msgsnd(msqid, &buf, len + 1, 0) == -1) {
printf("Error in sending the message");
exit(1);
}
return 0;
}