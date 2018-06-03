/*
 NAME: Saad Mushtaq
 COMP 3430 Operating System
 Assignment 1 Q1
*/
// reminder - compile with -lpthread
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define ROWS 4
#define COLS 5
#define COLS_IN_RESULTANT_MATRIX 2

// *******************Data Structure***************************
typedef struct Matrix {
    int row;
    int col;
}Matrix;

// ************************************************************
int matrix1 [4][5]= {
    {1, 2, 3, 4 , 5},
    {6, 7, 8, 9, 10},
    {11, 12, 13, 14, 15},
    {16, 17, 18, 19, 20}
};

//int matrix1[3][2] = { {1,4}, {2,5}, {3,6} };
//int matrix2[2][3] = { {8,7,6}, {5,4,3} };
int matrix2 [5][2] = {
    {1, 2},
    {3, 4},
    {5, 6},
    {7, 8},
    {9, 10}
};

int resultant_matrix[ROWS][COLS_IN_RESULTANT_MATRIX];

// ********************Thread**********************************
void* worker_thread(void* data) {
    
    Matrix *array = (Matrix*) data;
    int i, sum = 0;
    
    for (i = 0; i < COLS; i++) {
        sum += matrix1[array->row][i] * matrix2[i][array->col];
    }
    
    //assign the sum to its coordinate
    resultant_matrix[array->row][array->col] = sum;
    
    //Exit the thread
    pthread_exit(0);
}

void print_matrix() {
    int i, j;
    
    //Print out the resulting matrix
    for(i = 0; i < ROWS; i++) {
        for(j = 0; j < COLS_IN_RESULTANT_MATRIX; j++) {
            printf("%d ", resultant_matrix[i][j]);
        }
        printf("\n");
    }
}

// ************************************************************
int main(int argc, char *argv[]) {
    
    pthread_t thread;
    int i, j;
    
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS_IN_RESULTANT_MATRIX; j++) {
            Matrix *matrix = (Matrix*) malloc(sizeof(Matrix));
            matrix->row = i;
            matrix->col = j;
            
            // create the worker thread
            pthread_create(&thread, NULL, worker_thread, matrix);
        }
    }
    
    // suspend execution of the calling thread until the target thread terminates
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS_IN_RESULTANT_MATRIX; j++) {
            pthread_join(thread, NULL);
        }
    }
    
    print_matrix();
}
