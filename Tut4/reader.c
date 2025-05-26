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
// Get the message queue ID using key
if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
printf("Error in creating message queue");
exit(1);
}
// Read the message
if (msgrcv(msqid, &buf, sizeof(char) * 200, 0, 0) == -1) {
printf("Error in receiving the message");
exit(1);
}
printf("Reader: \"%s\"\n", buf.mtext);
// Delete the message queue
if (msgctl(msqid, IPC_RMID, NULL) == -1) {
printf("Error in deleting the message queue");
exit(1);
}
return 0;
}
