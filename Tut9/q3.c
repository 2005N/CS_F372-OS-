#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>

void print_policy(int policy) {
    switch (policy) {
        case SCHED_FIFO:
            printf("Policy: SCHED_FIFO\n");
            break;
        case SCHED_RR:
            printf("Policy: SCHED_RR\n");
            break;
        case SCHED_OTHER:
            printf("Policy: SCHED_OTHER\n");
            break;
        default:
            printf("Policy: UNKNOWN\n");
            break;
    }
}

void print_thread_info() {
    int policy;
    struct sched_param param;

    // Get current scheduling policy and priority
    pthread_getschedparam(pthread_self(), &policy, &param);
    print_policy(policy);
    printf("Priority: %d\n", param.sched_priority);
}

void *thread_function(void *arg) {
    printf("Initial scheduling policy and priority:\n");
    print_thread_info();

    // Set new scheduling policy and priority
    struct sched_param param;
    param.sched_priority = 10;  // Adjust priority as per requirement

    // Attempt to set to Round Robin policy (SCHED_RR) with new priority
    if (pthread_setschedparam(pthread_self(), SCHED_RR, &param) != 0) {
        perror("pthread_setschedparam failed");
        pthread_exit(NULL);
    }

    printf("\nUpdated scheduling policy and priority:\n");
    print_thread_info();

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;

    // Initialize thread attributes
    pthread_attr_init(&attr);

    // Set the scheduling attribute to explicit (not inherited from the main thread)
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    // Create the thread with specified attributes
    if (pthread_create(&thread, &attr, thread_function, NULL) != 0) {
        perror("pthread_create failed");
        return EXIT_FAILURE;
    }

    // Wait for the thread to complete
    pthread_join(thread, NULL);

    // Destroy thread attributes
    pthread_attr_destroy(&attr);

    return 0;
}

//compile and run 'sudo ./q3'