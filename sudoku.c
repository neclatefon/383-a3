#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 9
#define COLS 9
#define SUBGRID_SIZE 3

// Parameters struct for thread
typedef struct ThreadParams {
    char* sudoku;
    int result;
} ThreadParams;

void readSudokuFromFile(char* fileName, char grid[9][9]);
void printSudoku(char grid[9][9]);
void* validateRows(void* arg);
void* validateColumns(void* arg);
void* validateGrid(void* arg);

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        puts("Not enough parameters passed."); //displaying the error message for insufficient parameters
        return 1;
    }

    char fileName[255], sudokuGrid[9][9];
    fileName[0] = '\0';

    strcat(fileName, "./");
    strcat(fileName, argv[1]);

    readSudokuFromFile(fileName, sudokuGrid);
    printSudoku(sudokuGrid);

    pthread_t threadIds[27];
    ThreadParams params[27];
    int result = 1;

    for (int i = 0; i < 27; i++) {
        params[i].sudoku = (char*)sudokuGrid;
        params[i].result = 1;

        if (i < 9) {
            params[i].sudoku = (char*)sudokuGrid;
            pthread_create(&threadIds[i], NULL, validateRows, &params[i]);
        } else if (i < 18) {
            params[i].sudoku = (char*)sudokuGrid;
            pthread_create(&threadIds[i], NULL, validateColumns, &params[i]);
        } else {
            params[i].sudoku = (char*)sudokuGrid;
            pthread_create(&threadIds[i], NULL, validateGrid, &params[i]);
        }
    }

    for (int i = 0; i < 27; i++) {
        pthread_join(threadIds[i], NULL);
        if (params[i].result == 0) {
            result = 0;
            break;
        }
    }

    if (result) {
        puts("Sudoku puzzle is valid");
    } else {
        puts("Sudoku puzzle is invalid");
    }

    return 0;
}

void readSudokuFromFile(char* fileName, char grid[9][9]) {
    int fileDescriptor, bytesRead, row = 0, col = 0;
    char* token;
    char delimiter[] = " \n\r";
    char* buffer = malloc(100 * sizeof(char));

    if ((fileDescriptor = open(fileName, O_RDONLY, 0777)) < 0) {
        perror("File open error");
        return;
    }

    while ((bytesRead = read(fileDescriptor, buffer, 100)) != 0) {
        token = strtok(buffer, delimiter);

        while (token != NULL) {
            if (col == 9) {
                col = 0;
                row++;
            }
            grid[row][col++] = *token;
            token = strtok(NULL, delimiter);
        }

        for (int i = 0; i < 100; i++) {
            buffer[i] = '\0';
        }
    }

    free(buffer);
}

void printSudoku(char grid[9][9]) {
    puts("Sudoku Puzzle Solution is:");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (j == COLS - 1) {
                printf("%c\n", grid[i][j]);
            } else {
                printf("%c ", grid[i][j]);
            }
        }
    }
}

void* validateRows(void* arg) {
    ThreadParams* param = (ThreadParams*)arg;
    for (int i = 0; i < ROWS; i++) {
        char numbers[] = "123456789";
        for (int j = 0; j < COLS; j++) {
            char currentNum = param->sudoku[i * ROWS + j];
            if (currentNum != '.' && strchr(numbers, currentNum) != NULL) {
                int index = strchr(numbers, currentNum) - numbers;
                numbers[index] = '.';
            } else {
                param->result = 0;
                pthread_exit(NULL);
            }
        }
    }
    pthread_exit(NULL);
}

void* validateColumns(void* arg) {
    ThreadParams* param = (ThreadParams*)arg;
    for (int i = 0; i < COLS; i++) {
        char numbers[] = "123456789";
        for (int j = 0; j < ROWS; j++) {
            char currentNum = param->sudoku[j * ROWS + i];
            if (currentNum != '.' && strchr(numbers, currentNum) != NULL) {
                int index = strchr(numbers, currentNum) - numbers;
                numbers[index] = '.';
            } else {
                param->result = 0;
                pthread_exit(NULL);
            }
        }
    }
    pthread_exit(NULL);
}

void* validateGrid(void* arg) { //thread function to validate individual 3x3 grids
    ThreadParams* param = (ThreadParams*)arg;
    for (int i = 0; i < SUBGRID_SIZE; i++) {
        for (int j = 0; j < SUBGRID_SIZE; j++) {
            char numbers[] = "123456789";
            for (int k = 0; k < SUBGRID_SIZE; k++) {
                for (int l = 0; l < SUBGRID_SIZE; l++) {
                    char currentNum = param->sudoku[(i * SUBGRID_SIZE + k) * ROWS + (j * SUBGRID_SIZE + l)];
                    if (currentNum != '.' && strchr(numbers, currentNum) != NULL) {
                        int index = strchr(numbers, currentNum) - numbers;
                        numbers[index] = '.';
                    } else {
                        param->result = 0;
                        pthread_exit(NULL);
                    }
                }
            }
        }
    }
    pthread_exit(NULL);
}
