#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Function to calculate the factorial of a number
unsigned long long factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    else
        return n * factorial(n - 1);
}

int main() {
    int pipe_fd[3][2]; // Array of pipes for communication between parent and children
    int pipe_fd_back[3][2]; // Array of pipes for communication from children to parent
    pid_t pid[3];
    int numbers[3];
    unsigned long long result;

    // Input: Get three positive integers from the user
    printf("Enter three positive integers: ");
    for (int i = 0; i < 3; i++) {
        scanf("%d", &numbers[i]);
    }

    // Create pipes for each child process
    for (int i = 0; i < 3; i++) {
        if (pipe(pipe_fd[i]) == -1 || pipe(pipe_fd_back[i]) == -1) {
            perror("Error creating pipe");
            exit(1);
        }
    }

    // Create three child processes
    for (int i = 0; i < 3; i++) {
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("Error in fork");
            exit(1);
        }

        if (pid[i] == 0) { // Child process
            close(pipe_fd[i][1]); // Close write end of pipe to parent
            close(pipe_fd_back[i][0]); // Close read end of pipe to parent

            int num;
            read(pipe_fd[i][0], &num, sizeof(int)); // Read number from parent
            close(pipe_fd[i][0]); // Close read end of pipe

            // Calculate factorial
            result = factorial(num);

            // Send result back to parent
            write(pipe_fd_back[i][1], &result, sizeof(result));
            close(pipe_fd_back[i][1]); // Close write end of pipe

            exit(0); // Terminate child process after work is done
        } else { // Parent process
            close(pipe_fd[i][0]); // Close read end of pipe from child
            close(pipe_fd_back[i][1]); // Close write end of pipe from child

            // Send number to child
            write(pipe_fd[i][1], &numbers[i], sizeof(int));
            close(pipe_fd[i][1]); // Close write end of pipe

            // Wait for child to finish and receive result
            read(pipe_fd_back[i][0], &result, sizeof(result));
            close(pipe_fd_back[i][0]); // Close read end of pipe

            // Print the result
            printf("Factorial of %d is %llu\n", numbers[i], result);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}
