#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    char *filename;
    int thread_num;
    int start_index;
    int numbers_per_thread;
} ThreadData;

void *runner(void* param){
    ThreadData *data=(ThreadData*)param;
    FILE *file=fopen(data->filename,"r");
    if (file == NULL) {
        perror("Error opening file");
        pthread_exit(NULL);
    }
    int number;
    char line[20];
    int current_line=0;

    // Move the file pointer to the starting position for this thread
    fseek(file, 0, SEEK_SET);
    
    // Skip lines until the start_index
    while (current_line < data->start_index && fgets(line, sizeof(line), file)) {
        current_line++;
    }

    for(int i=0;i<data->numbers_per_thread;i++){
        if(fgets(line,sizeof(line),file)){
            number=atoi(line);
            printf("Thread %d processed number: %d\n", data->thread_num, number);
        }
    }
    fclose(file);
    pthread_exit(0);
}

int main(int argc,char *argv[]){
    char *filename = argv[1];
    int total_numbers = atoi(argv[2]);
    int numbers_per_thread = total_numbers / 3;

    pthread_t tid[3];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    ThreadData thread_data[3];

    for (int i = 0; i < 3; i++) {
        thread_data[i].filename=filename;
        thread_data[i].numbers_per_thread=numbers_per_thread;
        thread_data[i].start_index= i*numbers_per_thread;
        thread_data[i].thread_num=i;
        pthread_create(&tid[i], &attr, runner, &thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}