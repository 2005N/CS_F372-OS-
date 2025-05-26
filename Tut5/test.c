#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define SHM_KEY 0x1234
#define BUF_SIZE 8
int main() {
int shmid;
char *shmPtr;
if (fork() == 0) {
sleep(3); // To wait for the parent to write
// Get the shared memory ID
shmid = shmget(SHM_KEY, BUF_SIZE, 0644);
if (shmid == -1) {
perror("Shared memory");
return 1;
}
// Attach to the segment to get a pointer to it.
shmPtr = shmat(shmid, NULL, 0);
if (shmPtr == (void *)-1) {
perror("Shared memory attach");
return 1;
}
printf("Child: received message \"%s\"\n", shmPtr);
printf("Child: Reading Done, Detaching Shared Memory\n");
if (shmdt(shmPtr) == -1) {
perror("shmdt");
return 1;
}
} else {
shmid = shmget(SHM_KEY, BUF_SIZE, 0644 | IPC_CREAT);
if (shmid == -1) {
perror("Shared memory");
return 1;
}
// Attach to the segment to get a pointer to it.
shmPtr = shmat(shmid, NULL, 0);
if (shmPtr == (void *)-1) {
perror("Shared memory attach");
return 1;
}
sprintf(shmPtr, "%s", "Hello.");
printf("Parent: Writing Done, waiting for child\n");
wait(NULL);
if (shmdt(shmPtr) == -1) {
perror("shmdt");
return 1;
}
if (shmctl(shmid, IPC_RMID, 0) == -1) {
perror("shmctl");
return 1;
}
}
return 0;
}