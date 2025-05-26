#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int pipe_fd[2];
    pid_t pid;

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("error in creating pipe");
        exit(1);
    }

    // First fork to create the first child
    pid = fork();
    if (pid == -1) {
        perror("error in fork");
        exit(1);
    }

    if (pid == 0) { // Child process 1: "ls"
        // Redirect standard output to the write end of the pipe
        dup2(pipe_fd[1], 1);
        close(pipe_fd[0]); // Close unused read end
        close(pipe_fd[1]); // Close the original write end

        // Execute "ls"
        if (execlp("ls", "ls", NULL) == -1) {
            perror("error in execlp ls");
            exit(1);
        }
    } else { // Parent process
        // Second fork to create the second child
        pid = fork();
        if (pid == -1) {
            perror("error in fork");
            exit(1);
        }

        if (pid == 0) { // Child process 2: "wc -l"
            // Redirect standard input to the read end of the pipe
            dup2(pipe_fd[0], 0);
            close(pipe_fd[1]); // Close unused write end
            close(pipe_fd[0]); // Close the original read end

            // Execute "wc -l"
            if (execlp("wc", "wc", "-l", NULL) == -1) {
                perror("error in execlp wc");
                exit(1);
            }
        } else { // Parent process
            // Close both ends of the pipe
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            // Wait for both child processes to finish
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
