#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int count = 1;                  // Global variable to be read/written
int numberOfReader = 0;          // Global variable to track number of readers

pthread_mutex_t mutex_count;     // Mutex to control access to 'count' (used by writers)
pthread_mutex_t mutex_readcount; // Mutex to control access to 'numberOfReader' (used by readers)

void *reader(void *reader_no) {
    int reader_id = *((int *)reader_no);
    
    while (1) {
        // Reader entry section
        pthread_mutex_lock(&mutex_readcount); // Ensure mutual exclusion on reader count
        numberOfReader++;
        if (numberOfReader == 1) {
            pthread_mutex_lock(&mutex_count); // First reader locks writers
        }
        pthread_mutex_unlock(&mutex_readcount); // Allow other readers to proceed

        // Reading section (Critical Section for Readers)
        printf("Reader %d reads the value of count: %d\n", reader_id, count);

        // Reader exit section
        pthread_mutex_lock(&mutex_readcount); // Ensure mutual exclusion on reader count
        numberOfReader--;
        if (numberOfReader == 0) {
            pthread_mutex_unlock(&mutex_count); // Last reader unlocks writers
        }
        pthread_mutex_unlock(&mutex_readcount); // Allow other readers to proceed

        sleep(1); // Simulate processing time
    }
}

void *writer(void *writer_no) {
    int writer_id = *((int *)writer_no);

    while (1) {
        // Writing section (Critical Section for Writers)
        pthread_mutex_lock(&mutex_count); // Ensure no reader or other writer can access
        count *= 2;                        // Writer updates the shared variable
        printf("Writer %d doubles the value of count: %d\n", writer_id, count);
        pthread_mutex_unlock(&mutex_count); // Release access to the shared resource

        sleep(2); // Simulate processing time
    }
}

int main() {
    pthread_t read[5], write[5];
    pthread_mutex_init(&mutex_count, NULL);     // Initialize the mutex for 'count'
    pthread_mutex_init(&mutex_readcount, NULL); // Initialize the mutex for 'numberOfReader'

    int reader_id[5], writer_id[5];
    for (int i = 0; i < 5; i++) {
        reader_id[i] = i + 1;
        writer_id[i] = i + 1;
        pthread_create(&read[i], NULL, reader, (void *)&reader_id[i]);
        pthread_create(&write[i], NULL, writer, (void *)&writer_id[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(read[i], NULL);
        pthread_join(write[i], NULL);
    }

    pthread_mutex_destroy(&mutex_count);
    pthread_mutex_destroy(&mutex_readcount);

    return 0;
}
