#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <errno.h>

#define NANOSECONDS_PER_SECOND 1000000000L

// Function to calculate the time difference in nanoseconds
long diff_in_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * NANOSECONDS_PER_SECOND + (end.tv_nsec - start.tv_nsec);
}

int main() {
    struct timespec start, end;
    int policy = sched_getscheduler(0);

    if (policy == -1) {
        perror("sched_getscheduler failed");
        return EXIT_FAILURE;
    }

    // Check if the current scheduling policy is SCHED_RR
    if (policy != SCHED_RR) {
        printf("This program is not running under SCHED_RR.\n");
        printf("Run the program with: sudo chrt --rr <priority> ./<exec_name>\n");
        return EXIT_FAILURE;
    }

    printf("Calculating time slice for SCHED_RR...\n");

    // Record the start time
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime failed");
        return EXIT_FAILURE;
    }

    // Yield control to allow another process to run
    sched_yield();

    // Record the end time
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime failed");
        return EXIT_FAILURE;
    }

    // Calculate and print the time difference
    long time_slice = diff_in_ns(start, end);
    printf("Estimated time slice (in nanoseconds): %ld ns\n", time_slice);

    return EXIT_SUCCESS;
}

//compile and run 'sudo chrt --rr 1 ./q2'