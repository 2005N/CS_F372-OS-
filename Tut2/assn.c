#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/msg.h>

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

// Function to count occurrences of a word in a file
int count_word_occurrences(FILE* file, const char* target_word, int max_word_length) {
    char word[max_word_length];
    int count = 0;

    rewind(file);  // Reset file pointer to the beginning

    // Read each word from the file
    while (fscanf(file, "%s", word) != EOF) {
        if (strcmp(word, target_word) == 0) {  // Ensure exact match, no substrings
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

    message.mtype = 1;    // mtype 1 for sending
    message.data = sum;   // Send the sum of occurrences
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

    // Handle the first diagonal
    int x = count_word_occurrences(words_file, matrix[0][0], max_word_length);
    printf("first word occurrence: %d\n", x);
    int key = get_key_from_helper(msgq_key, x);
    printf("first key: %d\n", key);
    
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

        printf("Processing diagonal %d (starting at i=%d, j=%d)\n", d, i, j);

        // Traverse the diagonal
        while (i >= 0 && j < N) {
         
            printf("before: '%s'\n", matrix[i][j]);
            decode(matrix[i][j],(long) key);  // Decode the copied word
            printf("Decoded word at (%d, %d): '%s'\n", i, j, matrix[i][j]);

            rewind(words_file);  // Reset file pointer for counting
            count += count_word_occurrences(words_file, matrix[i][j], max_word_length);
            printf("count: %d\n", count);

            i--;  // Move up the row
            j++;  // Move right in the column
        }

        // Send the sum to the helper process and get the next key
        key = get_key_from_helper(msgq_key, count);
        printf("Key after diagonal %d: %d\n\n", d, key);

        
    }

    shmdt(matrix);
    fclose(input_file);
    fclose(words_file);

    return 0;
}
