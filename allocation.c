#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct FreeNode {
    int size;
    int start;
    int end;
    struct FreeNode *next;
} FREE;

typedef struct AllocatedNode {
    int size;
    char *proc_name;
    unsigned char *memory;
    int start;
    int end;
    struct AllocatedNode *next;
} ALLOCATED;

void allocateMemory(FREE **freeMem, ALLOCATED **allocMem, char alg, char *procName, int size);
void freeMemory(FREE **freeMem, ALLOCATED **allocMem, char *procName);

void freeInsert(FREE **freeMem, FREE *node) {
    if (*freeMem) {
        FREE *ptr = *freeMem;
        if (node->start < ptr->start) {
            node->next = ptr;
            *freeMem = node;
        } else {
            while (ptr->next) {
                if (node->start < ptr->next->start) {
                    node->next = ptr->next;
                    ptr->next = node;
                    return;
                } else {
                    ptr = ptr->next;
                }
            }
            if (ptr->next == NULL) {
                ptr->next = node;
            }
        }
    } else {
        *freeMem = node;
    }
}

void allocInsert(ALLOCATED **allocMem, ALLOCATED *node) {
    if (*allocMem) {
        ALLOCATED *ptr = *allocMem;
        if (node->start < ptr->start) {
            node->next = ptr;
            *allocMem = node;
        } else {
            while (ptr->next) {
                if (node->start < ptr->next->start) {
                    node->next = ptr->next;
                    ptr->next = node;
                    return;
                } else {
                    ptr = ptr->next;
                }
            }
            if (ptr->next == NULL) {
                ptr->next = node;
            }
        }
    } else {
        *allocMem = node;
    }
}

unsigned char *memory;
int totalMemory;
int usedMemory = 0;

void clean(FREE **freeMem) {
    FREE *curr = *freeMem;
    FREE *prev = NULL;

    while (curr) {
        if (curr->size == 0) {
            if (prev) {
                prev->next = curr->next;
            } else {
                *freeMem = curr->next;
            }
        }

        if (prev) {
            if (prev->end == curr->start - 1) {
                prev->end = curr->end;
                prev->size += curr->size;
                prev->next = curr->next;
                free(curr);
                curr = prev->next;
            }
        }

        prev = curr;
        curr = curr->next;
    }
}

int firstFit(FREE **freeMem, int size) {
    FREE *ptr = *freeMem;
    int start;
    if (ptr) {
        while (ptr) {
            if (ptr->size >= size) {
                start = ptr->start;
                ptr->start += size;
                ptr->size -= size;
                return start;
            }
            ptr = ptr->next;
        }
    }
    return -1;
}

void allocateMemory(FREE **freeMem, ALLOCATED **allocMem, char alg, char *procName, int size) {
    int block;
    if (alg == 'F') {
        block = firstFit(freeMem, size);
    } else {
        printf("Invalid fit algorithm\n");
        return;
    }
    if (block == -1) {
        printf("No hole of sufficient size\n");
        return;
    } else {
        ALLOCATED *newNode = (ALLOCATED *)malloc(sizeof(ALLOCATED));
        newNode->start = block;
        newNode->end = block + size - 1;
        newNode->size = size;
        newNode->memory = memory + block;
        newNode->proc_name = procName;

        newNode->next = NULL;

        allocInsert(allocMem, newNode);
        clean(freeMem);

        usedMemory += size;
        printf("Successfully allocated %d to process %s\n", size, procName);
    }
}

void freeMemory(FREE **freeMem, ALLOCATED **allocMem, char *procName) {
    ALLOCATED *curr = *allocMem;
    ALLOCATED *prev = NULL;

    while (curr) {
        if (strcmp(curr->proc_name, procName) == 0) {
            break;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }

    if (curr == NULL) {
        printf("There is no process of name: %s", procName);
        return;
    } else {
        int freeSize = curr->size;
        int start = curr->start;

        if (prev) {
            prev->next = curr->next;
        } else {
            *allocMem = curr->next;
        }
        free(curr->proc_name);
        free(curr);

        FREE *newNode = (FREE *)malloc(sizeof(FREE));
        newNode->size = freeSize;
        newNode->start = start;
        newNode->end = start + freeSize - 1;
        freeInsert(freeMem, newNode);
        clean(freeMem);

        usedMemory -= freeSize;
        printf("Releasing memory for process %s\n", procName);
    }
}

int main(int argc, char *argv[]) {
    FREE *freeMem = NULL;
    ALLOCATED *allocMem = NULL;
    char command[20];
    totalMemory = 1048576;
    memory = (char *)malloc(totalMemory * sizeof(char));

    freeMem = (FREE *)malloc(sizeof(FREE));
    freeMem->size = totalMemory;
    freeMem->start = 0;
    freeMem->end = totalMemory - 1;
    freeMem->next = NULL;

    printf("Allocated %d bytes of memory\n", totalMemory);
    printf("Here, the First fit approach has been implemented and the allocated %d bytes of memory.\n", totalMemory);
    printf("allocator>");

    while (scanf("%[^\n]%*c", command)) {
        if (strcmp(command, "Exit") == 0) {
            free(memory);
            exit(0);
        }

        if (strcmp(command, "Status") == 0) {
            printf("Partitions [Allocated memory = %d]:\n", usedMemory);
            ALLOCATED *ptr = allocMem;
            while (ptr) {
                printf("Address [%d:%d] Process %s\n", ptr->start, ptr->end, ptr->proc_name);
                ptr = ptr->next;
            }

            printf("\nHoles [Free memory = %d]:\n", totalMemory - usedMemory);
            FREE *fptr = freeMem;
            while (fptr) {
                printf("Address [%d:%d] len = %d\n", fptr->start, fptr->end, fptr->size);
                fptr = fptr->next;
            }
        } else {
            char *token = strtok(command, " ");

            if (strcmp(token, "RQ") == 0) {
                char processNum[4];
                int processSize;

                token = strtok(NULL, " ");
                strncpy(processNum, token, sizeof(processNum));
                token = strtok(NULL, " ");
                processSize = atoi(token);
                token = strtok(NULL, " ");
                char type = *token;

                allocateMemory(&freeMem, &allocMem, type, processNum, processSize);
            } else if (strcmp(token, "RL") == 0) {
                char processNum[4];

                token = strtok(NULL, " ");
                strncpy(processNum, token, sizeof(processNum));
                freeMemory(&freeMem, &allocMem, processNum);
            } else if (strcmp(token, "C") == 0) {
                ALLOCATED *curr = allocMem;
                ALLOCATED *prev = NULL;
                while (curr) {
                    if (prev && prev->end != curr->start - 1) {
                        curr->start = prev->end + 1;
                        curr->end = curr->start + curr->size - 1;
                        curr->memory = memory + curr->start;
                    } else if (prev == NULL && curr->start != 0) {
                        curr->start = 0;
                        curr->end = curr->size - 1;
                        curr->memory = memory;
                    }
                    prev = curr;
                    curr = curr->next;
                }

                FREE *currFree = freeMem;
                FREE *prevFree = NULL;

                freeMem = (FREE *)malloc(sizeof(FREE));
                freeMem->size = totalMemory - usedMemory;
                freeMem->start = usedMemory;
                freeMem->end = usedMemory + freeMem->size - 1;
                freeMem->next = NULL;

                while (currFree) {
                    prevFree = currFree;
                    currFree = currFree->next;
                    free(prevFree);
                }

                printf("Compaction process is successful\n");
            }
        }
        printf("allocator>");
    }

    return 0;
}
