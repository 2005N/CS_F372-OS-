#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5  // Buffer capacity
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3

int buffer[BUFFER_SIZE];  // Shared buffer
int in = 0, out = 0;      // Buffer index pointers

// Semaphores for synchronization
sem_t empty;  // Tracks empty slots in the buffer
sem_t full;   // Tracks full slots in the buffer
pthread_mutex_t mutex;  // Mutex to protect access to the buffer

// Producer function
void *producer(void *arg) {
    int id = *((int *)arg);
    int item;

    while (1) {
        // Produce an item (simulated)
        item = rand() % 100;  // Producing a random item
        sleep(1);  // Simulate time taken to produce

        // Wait if the buffer is full (empty slots are 0)
        sem_wait(&empty);

        // Lock the buffer for mutual exclusion
        pthread_mutex_lock(&mutex);

        // Add the item to the buffer
        buffer[in] = item;
        printf("Producer %d produced item %d at buffer[%d]\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;  // Circular buffer logic

        // Unlock the buffer and signal that a slot is full
        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);  // Simulate time between productions
    }
}

// Consumer function
void *consumer(void *arg) {
    int id = *((int *)arg);
    int item;

    while (1) {
        // Wait if the buffer is empty (full slots are 0)
        sem_wait(&full);

        // Lock the buffer for mutual exclusion
        pthread_mutex_lock(&mutex);

        // Remove an item from the buffer
        item = buffer[out];
        printf("Consumer %d consumed item %d from buffer[%d]\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;  // Circular buffer logic

        // Unlock the buffer and signal that a slot is empty
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        sleep(1);  // Simulate time taken to process the item
    }
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS], consumer_ids[NUM_CONSUMERS];

    // Initialize the semaphores
    sem_init(&empty, 0, BUFFER_SIZE);  // Initially, all buffer slots are empty
    sem_init(&full, 0, 0);             // Initially, no buffer slots are full
    pthread_mutex_init(&mutex, NULL);  // Initialize the mutex

    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for producers and consumers (infinite loop in this case)
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Destroy semaphores and mutex (though unreachable in this case)
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
