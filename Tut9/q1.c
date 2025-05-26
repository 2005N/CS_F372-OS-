#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

void print_priority_range(int policy) {
    int min_priority = sched_get_priority_min(policy);
    int max_priority = sched_get_priority_max(policy);
    if (min_priority == -1 || max_priority == -1) {
        perror("Failed to get priority range");
        exit(EXIT_FAILURE);
    }
    printf("Valid priority range for policy: %d to %d\n", min_priority, max_priority);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <policy> <priority> <pid>\n", argv[0]);
        fprintf(stderr, "Policy options: 'f' for FIFO, 'r' for RR, 'o' for OTHER\n");
        return EXIT_FAILURE;
    }

    int policy;
    if (argv[1][0] == 'f') {
        policy = SCHED_FIFO;
    } else if (argv[1][0] == 'r') {
        policy = SCHED_RR;
    } else if (argv[1][0] == 'o') {
        policy = SCHED_OTHER;
    } else {
        fprintf(stderr, "Invalid policy: use 'f' for FIFO, 'r' for RR, or 'o' for OTHER\n");
        return EXIT_FAILURE;
    }

    int priority = atoi(argv[2]);
    pid_t pid = atoi(argv[3]);

    print_priority_range(policy);

    struct sched_param param;
    param.sched_priority = priority;

    if (sched_setscheduler(pid, policy, &param) == -1) {
        perror("sched_setscheduler failed");
        return EXIT_FAILURE;
    }

    printf("Scheduling policy and priority changed successfully.\n");
    return EXIT_SUCCESS;
}

//compile and run 'sleep 10000 &'   'sudo ./q1 f 10 <PID>'  'chrt -p <PID>'