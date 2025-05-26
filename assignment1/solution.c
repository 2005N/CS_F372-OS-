#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/msg.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024  // Size of the buffer to store words

struct msg_buffer {
    long mtype;
    int data;
};

void decode(char *word, long x) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] >= 'a' && word[i] <= 'z') {
            word[i] = (char)((((int)(word[i] - 'a' + x) % 26) + 26) % 26 + 'a');
        }
    }
}

// Function to build a word frequency hash table from a file
void build_word_frequency(FILE *file, int max_word_length, char ***words, int *word_count) {
    char word[max_word_length];
    int capacity = BUFFER_SIZE;
    *word_count = 0;
    
    *words = malloc(capacity * sizeof(char *));
    if (!*words) {
        perror("malloc");
        exit(1);
    }
    
    // Read each word from the file
    while (fscanf(file, "%s", word) != EOF) {
        if (*word_count >= capacity) {
            capacity *= 2;
            *words = realloc(*words, capacity * sizeof(char *));
            if (!*words) {
                perror("realloc");
                exit(1);
            }
        }
        (*words)[*word_count] = malloc(strlen(word) + 1);  
        if (!(*words)[*word_count]) {
        perror("malloc");
        exit(1);
        }

        // Copy the word into the allocated memory
        strcpy((*words)[*word_count], word);
        (*word_count)++;
    }
}

// Function to count occurrences of a word in the buffered words
int count_word_occurrences(char **words, int word_count, char *target_word) {
    int count = 0;
    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i], target_word) == 0) {
            count++;
        }
    }
    return count;
}

// Function to communicate with the helper process
int get_key_from_helper(int msgq_key, int sum) {
    int msgqid;
    struct msg_buffer message;

    msgqid = msgget(msgq_key, 0666 | IPC_CREAT);
    if (msgqid < 0) {
        perror("msgget");
        exit(1);
    }
    // mtype 1 for sending and 2 for receiving
    message.mtype = 1;    
    message.data = sum;  
    if (msgsnd(msgqid, &message, sizeof(message)-sizeof(message.mtype), 0) < 0) {
        perror("msgsnd");
        exit(1);
    }

   if (msgrcv(msgqid, &message, sizeof(message)-sizeof(message.mtype), 2, 0) < 0) {
        perror("msgrcv");
        exit(1);
    }

    return message.data;  // key received from the helper
}

int main(int argc, char *argv[]) {
    int t = atoi(argv[1]); // Convert the argument to an integer
    char input_filename[20], words_filename[20];

    // Create the filenames based on the command-line argument
    sprintf(input_filename, "input%d.txt", t);
    sprintf(words_filename, "words%d.txt", t);

    // Open the input and words files
    FILE *input_file = fopen(input_filename, "r");
    FILE *words_file = fopen(words_filename, "r");

    if (input_file == NULL || words_file == NULL) {
        printf("Could not open one of the files.\n");
        return 1;
    }
    
    int N, max_word_length, shm_key, msgq_key;
    
    fscanf(input_file, "%d", &N);               // Read matrix size (N)
    fscanf(input_file, "%d", &max_word_length); // Read max length of a word
    fscanf(input_file, "%d", &shm_key);         // Read shared memory key
    fscanf(input_file, "%d", &msgq_key);        // Read message queue key

    int shm_id = shmget(shm_key, sizeof(char[N][N][max_word_length]), 0666 | IPC_CREAT);
    if (shm_id < 0) {
        perror("shmget");
        return 1;
    }

    // Attach to the shared memory
    char (*matrix)[N][max_word_length] = shmat(shm_id, NULL, 0);
    if (matrix == (void *) -1) {
        perror("shmat");
        return 1;
    }

    // Build word frequency buffer
    char **word_list;
    int word_count;
    build_word_frequency(words_file, max_word_length, &word_list, &word_count);
    fclose(words_file);

    // Handle the first diagonal
    int x = count_word_occurrences(word_list, word_count, matrix[0][0]);
    int key = get_key_from_helper(msgq_key, x);    
    int count;
    for (int d = 1; d < (2 * N - 1); d++) {
        count = 0; 
        int i, j;

        // Check for first or second half of the diagonals
        if (d < N) {
            i = d;  // For first half diagonals
            j = 0;
        } else {
            i = N - 1;  // For second half diagonals
            j = d - N + 1;
        }

        // Traverse the diagonal
        while (i >= 0 && j < N) {
        
            decode(matrix[i][j], (long) key);  // Decode the word

            count += count_word_occurrences(word_list, word_count, matrix[i][j]);

            i--;  // Move up the row
            j++;  // Move right in the column
        }

        key = get_key_from_helper(msgq_key, count);
    }

    // Clean up
    shmdt(matrix);
    free(word_list); 

    return 0;
}
