#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    // Create a child process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        printf("Child process: Displaying the contents of the file %s\n", argv[1]);
        execlp("cat", "cat", argv[1],NULL);

        // If execlp returns, it must have failed
        perror("execlp failed in child process");
        exit(EXIT_FAILURE);
    } else {
        // Parent process waits for child to complete
        wait(NULL);

        // Parent process
        printf("Parent process: Listing files in the current directory\n");
        execlp("ls", "ls", NULL);

        // If execlp returns, it must have failed
        perror("execlp failed in parent process");
        exit(EXIT_FAILURE);
    }

    return 0;
}
