#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ThreadInfo {
    int process_id;
    int arr_time;
    int burst_time;
    int completion_time;
    int wait_time;
    int turn_around_time;
} ThreadInfo;

void loadInputFromFile(char *fileName, ThreadInfo **threads, int *threadCount);
void executeFCFS(ThreadInfo *threads, int threadCount);
void calculateAverages(ThreadInfo *threads, int threadCount);
void displayResults(ThreadInfo *threads, int threadCount);

float averageWaitTime;
float averageTurnaroundTime;

int main(int argc, char *args[]) {
    if (argc < 2) {
        printf("Input file name missing!!!\n"); //script is missing.. exiting
        return -1;
    }

    ThreadInfo *threads = NULL;
    int threadCount = 0;
    loadInputFromFile(args[1], &threads, &threadCount);
    executeFCFS(threads, threadCount);
    calculateAverages(threads, threadCount);
    displayResults(threads, threadCount);

    free(threads);

    return 0;
}

void loadInputFromFile(char *fileName, ThreadInfo **threads, int *threadCount) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        printf("Error opening the file!!!\n"); 
        exit(-1);
    }

    char line[32];
    *threadCount = 0;

    while (fgets(line, 32, file) != NULL) { //read the file and count the characters
        (*threadCount)++;
    }

    *threads = (ThreadInfo *)malloc(sizeof(ThreadInfo) * (*threadCount));
    fseek(file, 0, SEEK_SET);
    int index = 0;

    while (fgets(line, 32, file) != NULL) {
        sscanf(line, "%d,%d,%d", &(*threads)[index].process_id, &(*threads)[index].arr_time, &(*threads)[index].burst_time);
        index++;
    }

    fclose(file);
}

void executeFCFS(ThreadInfo *threads, int threadCount) { //executing fcfs
    int currentTime = 0;

    for (int i = 0; i < threadCount; i++) {
        if (threads[i].arr_time > currentTime) { //wait until they arrive
            currentTime = threads[i].arr_time;
        }

        threads[i].completion_time = currentTime + threads[i].burst_time;
        threads[i].turn_around_time = threads[i].completion_time - threads[i].arr_time;
        threads[i].wait_time = threads[i].turn_around_time - threads[i].burst_time;
        currentTime = threads[i].completion_time;
    }
}

void calculateAverages(ThreadInfo *threads, int threadCount) {
    float waitTimeTotal = 0;
    float turnAroundTotal = 0;

    for (int i = 0; i < threadCount; i++) {
        waitTimeTotal += threads[i].wait_time;
        turnAroundTotal += threads[i].turn_around_time;
    }

    averageWaitTime = waitTimeTotal / threadCount;
    averageTurnaroundTime = turnAroundTotal / threadCount;
}

void displayResults(ThreadInfo *threads, int threadCount) {
    printf("Thread ID  Arrival Time  Burst Time  Completion Time  Turn-Around Time  Waiting Time\n");

    for (int i = 0; i < threadCount; i++) {
        printf("%9d %13d %11d %16d %15d %13d\n", threads[i].process_id, threads[i].arr_time, threads[i].burst_time,
               threads[i].completion_time, threads[i].turn_around_time, threads[i].wait_time);
    }

    printf("The average waiting time: %.2f\n", averageWaitTime);
    printf("The average turn-around time: %.2f\n", averageTurnaroundTime);
}
