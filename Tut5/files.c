#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SHM_KEY 0x1234
#define BUF_SIZE 8 

void read_file_into_shared_memory(const char *filename, char *shmptr) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    // Read file contents into shared memory
    fread(shmptr, sizeof(char), BUF_SIZE, file);
    fclose(file);
}

int main(int argc, char *argv[]) {
    int shmid1, shmid2;
    char *shmptr1, *shmptr2;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <file1> <file2> <output_file>\n", argv[0]);
        return 1;
    }

    // Create shared memory segments for each child
    shmid1 = shmget(SHM_KEY, BUF_SIZE, 0644 | IPC_CREAT);
    shmid2 = shmget(SHM_KEY + 1, BUF_SIZE, 0644 | IPC_CREAT);
    if (shmid1 == -1 || shmid2 == -1) {
        perror("shmget");
        return 1;
    }

    // Fork the first child process
    if (fork() == 0) {
        shmptr1 = shmat(shmid1, NULL, 0);
        if (shmptr1 == (void *)-1) {
            perror("shmat");
            return 1;
        }

        read_file_into_shared_memory(argv[1], shmptr1);
        exit(0);
    }

    // Fork the second child process
    if (fork() == 0) {
        shmptr2 = shmat(shmid2, NULL, 0);
        if (shmptr2 == (void *)-1) {
            perror("shmat");
            return 1;
        }

        read_file_into_shared_memory(argv[2], shmptr2);
        exit(0);
    }

    // Wait for both children to finish
    wait(NULL);
    wait(NULL);

    // Parent process: Combine the contents and write to the output file
    shmptr1 = shmat(shmid1, NULL, 0);
    shmptr2 = shmat(shmid2, NULL, 0);
    if (shmptr1 == (void *)-1 || shmptr2 == (void *)-1) {
        perror("shmat");
        return 1;
    }

    FILE *outfile = fopen(argv[3], "w");
    if (!outfile) {
        perror("fopen");
        return 1;
    }

    fwrite(shmptr1, sizeof(char), strlen(shmptr1), outfile);
    fwrite(shmptr2, sizeof(char), strlen(shmptr2), outfile);
    fclose(outfile);

    // Detach and remove shared memory segments
    shmdt(shmptr1);
    shmdt(shmptr2);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);

    return 0;
}
