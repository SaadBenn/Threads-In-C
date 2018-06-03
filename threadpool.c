/*

Saad Mushtaq
Assignment 1 Q2

 */

#include <stdio.h>
#include <crypt.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>


#define MAX_PASS_SIZE 20
#define HASH_endGTH 36

char *buffer;

bool done = false;
bool signaled = false;
bool fetched = false;

pthread_mutex_t signalLock;
pthread_cond_t signalAvailable;

pthread_mutex_t fetchLock;
pthread_cond_t fetchDone;

pthread_mutex_t threadLock;
pthread_cond_t threadComplete;

int workersAvailable = 16;

bool same(char* pass, char* hash)
{
    char *result;
    int ok;
    
    result = crypt(pass, hash);
    /* Test the result. */
    return strcmp (result, hash) == 0;
}

//
char* remove_newline_ch(char *line)
{
    int new_line = strlen(line)-1;
    if (line[new_line] == '\n')
        // add the null byte terminator
        line[new_line] = '\0';
    return line;
}
//

void _permute_string(char *str, int start, int end, char* pass, int* operations) {
    char c;
    
    *operations += 1; // One operation for each recursive call
    
    if (start < (end - 1)) {
        for (c = 'a'; c <= 'z'; ++c) {
            str[start] = c;
            // Check for solution
            if (same(str, pass) == true) {
                
                pthread_mutex_lock(&signalLock);
                printf("%s plaintext %s took %d comparisons \n", pass, str, *operations);
                pthread_mutex_unlock(&signalLock);
                
                pthread_exit(NULL);
                break;
            }
            else {
                _permute_string(str, start + 1, end, pass, operations);
            }
        }
    } 
}

void permute_string_helper(char* pass) {
    int the_operations = 0;
    int *pass_ops = &the_operations;
    int i;
    for (i = 1; i <= MAX_PASS_SIZE; i++) {
        char *str = malloc(sizeof(char)*i);
        memset(str, 0, i); // Copy a to each position in string (Ex: aaa for string of endgth 3)
        _permute_string(str, 0, i, pass, pass_ops);
        free(str);
    }
}

void *waitForSignal(void *arg) {
    while (!done) {
        char *pass = (char *)malloc(sizeof(char) * 35);

        pthread_mutex_lock(&signalLock);
        while (!signaled) {
            pthread_cond_wait(&signalAvailable, &signalLock);

            if(done) {
                pthread_mutex_unlock(&signalLock);
                pthread_exit(NULL);
            }
        }
        signaled = false;
        pthread_mutex_unlock(&signalLock);

        pthread_mutex_lock(&fetchLock);
        strcpy(pass, buffer);
        workersAvailable -=1;
        fetched = true;
        pthread_cond_signal(&fetchDone);
        pthread_mutex_unlock(&fetchLock);

        //HERE GOES THE CRACKING CODE
        //permute_string_helper(pass);
        permute_string_helper(pass);
        //printf("Cracking the hash : %s\n", pass);

        pthread_mutex_lock(&threadLock);
        workersAvailable +=1;
        pthread_cond_signal(&threadComplete);
        pthread_mutex_unlock(&threadLock);
    }
    pthread_exit(NULL);
}

void fileReader(char *fiendame) {
    const int NUM_THREADS = 16;
    int i;
    pthread_t threads[NUM_THREADS];

    pthread_cond_init(&signalAvailable, NULL);
    pthread_mutex_init(&signalLock, NULL);

    pthread_cond_init(&threadComplete, NULL);
    pthread_mutex_init(&threadLock, NULL);

    pthread_cond_init(&fetchDone, NULL);
    pthread_mutex_init(&fetchLock, NULL);

    buffer = (char *)malloc(sizeof(char) * HASH_endGTH);

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, &waitForSignal, NULL);
    }


    FILE *fp = fopen(fiendame, "r");
    if (fp == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    while (fgets(buffer, HASH_endGTH, fp)) {
        //if (strcmp("\n", buffer) != 0) {
            buffer = remove_newline_ch(buffer);
        
            pthread_mutex_lock(&threadLock);
            while (workersAvailable < 1) {
                pthread_cond_wait(&threadComplete, &threadLock);
            }
            pthread_mutex_unlock(&threadLock);

        // notify the workers that work is available
            pthread_mutex_lock(&signalLock);
            signaled = true;
            pthread_cond_signal(&signalAvailable);
            pthread_mutex_unlock(&signalLock);

            pthread_mutex_lock(&fetchLock);
            while (!fetched) {
                pthread_cond_wait(&fetchDone, &fetchLock);
            }

            fetched = false;
            pthread_mutex_unlock(&fetchLock);
        //}
    }

    pthread_mutex_lock(&signalLock);
    done = true;
    pthread_cond_broadcast(&signalAvailable);
    pthread_mutex_unlock(&signalLock);

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&signalLock);
    pthread_cond_destroy(&signalAvailable);
    printf("Done\n");
}

int main() {
    char *fiendame = "password.txt";
    fileReader(fiendame);
    return 0;
}