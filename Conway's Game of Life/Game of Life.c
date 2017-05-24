//
//  main.c
//  2nd_Assignment_for_PP
//
//  Created by Raphae Don on 2017/5/18.
//  Copyright © Jahr 2017 Raphae Don. All rights reserved.
//
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))
#define DATA_COL 5

int data[5][5] =
{
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0 },
};

int malloc2dint(int ***array, int rows, int cols) {
    /* allocate the n*m contiguous items */
    int *p = (int *)malloc(rows*cols*sizeof(int));
    if (!p) return -1;
    
    /* allocate the row pointers into the memory */
    (*array) = (int **)malloc(rows*sizeof(int*));
    if (!(*array)) {
        free(p);
        return -1;
    }
    
    /* set up the pointers into the contiguous memory */
    for (int i=0; i<rows; i++)
        (*array)[i] = &(p[i*cols]);
    
    return 0;
}

int free2dint(int ***array) {
    /* free the memory - the first element of the array is at the start */
    free(&((*array)[0][0]));
    
    /* free the pointers into the memory */
    free(*array);
    
    return 0;
}

void Construct_New_Data(int** array, int old_data[][DATA_COL], int rows, int cols){
    // add a row or column to each matrix's edge
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if ( row == 0 || row == (rows-1)) {
                array[row][col] = 0;
            }
            else{
                if (col == 0 || col == (cols-1)) {
                    array[row][col] = 0;
                }
                else {
                    array[row][col] = old_data[row-1][col-1];
                }
            }
        }
    }
}

void Calculate_Sum_of_Data(int* data, int* sumArray, int* neighboursArray, int datalength) {
    if ( NULL != data ) {
        int sum = 0;
        int neighbour = 0;
        for (int i = 0; i < datalength; i++) {
            sum = data[i]+data[i+1]+data[i+2];
            neighbour = data[i]+data[i+2];
            sumArray[i] = sum;
            neighboursArray[i] = neighbour;
        }
    }
}

void Calculate_Next_States(int* org_data, int* neighbour, int* pre_sum, int* next_sum, int sum_len, int* result)
{
    if ( NULL != pre_sum && NULL != next_sum && NULL != neighbour ) {
        for (int i = 0; i < sum_len; i++) {
            
            int sum = pre_sum[i%sum_len] + neighbour[i%sum_len] + next_sum[i%sum_len];
            if ( sum < 2 && (org_data[i+1] == 1) ) {
                result[i%sum_len] = 0;
            }
            else if ( ( sum == 2 || sum == 3) && (org_data[i+1] == 1) ) {
                result[i%sum_len] = 1;
            }
            else if ( sum > 3 && (org_data[i+1] == 1) ) {
                result[i%sum_len] = 0;
            }
            else if ( sum == 3 && (org_data[i+1] == 0)) {
                result[i%sum_len] = 1;
            }
            else{
                result[i%sum_len] = 0;
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    // Initialize the MPI enviroment
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get current processor rank
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    
    int old_rows = SIZEOF(data);
    int old_cols = SIZEOF(data[0]);
    int new_rows = old_rows+2;
    int new_cols = old_cols+2;
    
    int** new_data = NULL;
    int result = malloc2dint(&new_data, new_rows, new_cols);
    if ( -1 == result ) {
        printf("new matrix allocation failed");
        return 0;
    }
    
    if ( current_rank == 0 ) {
        printf("rows: %d, columns: %d\n", old_rows, old_cols);
        printf("initial states:\n");
        for (int row = 0; row < old_rows; row++) {
            for (int col = 0; col < old_cols; col++) {
                printf("%d, ", data[row][col]);
            }
            printf("\n");
        }
        
        Construct_New_Data(new_data, data, new_rows, new_cols);
    }
    
    MPI_Bcast(&(new_data[0][0]), new_rows*new_cols, MPI_INT, 0, MPI_COMM_WORLD);
    
    printf("new rows: %d, new columns: %d\n", new_rows, new_cols);
    printf("extended matrix:\n");
    for (int row = 0; row < new_rows; row++) {
        for (int col = 0; col < new_cols; col++) {
            printf("%d, ", new_data[row][col]);
        }
        printf("\n");
    }

    int* sum_array = (int *)malloc(sizeof(int)*old_cols);
    int* neighbour_array = (int *)malloc(sizeof(int)*old_cols);
    int* data_row = new_data[current_rank];
    
    Calculate_Sum_of_Data(data_row, sum_array, neighbour_array, old_cols);
    MPI_Barrier(MPI_COMM_WORLD);
    int* pre_sum = (int *)malloc(old_cols*sizeof(int));
    int next = (current_rank + 1)% world_size;
    int prev = (world_size+current_rank - 1)% world_size;
    MPI_Sendrecv(sum_array, old_cols, MPI_INT, next, 0, pre_sum, old_cols, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    int* next_sum = (int *)malloc(old_cols*sizeof(int));
    MPI_Sendrecv(sum_array, old_cols, MPI_INT, prev, 0, next_sum, old_cols, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Barrier(MPI_COMM_WORLD);

    int* next_state = (int *)malloc(sizeof(int)*old_cols);
    memset(next_state,0,old_cols);
    Calculate_Next_States(data_row, neighbour_array, pre_sum, next_sum, old_cols, next_state);
    
    if ( 0 == current_rank ) {
        int* next_state = (int *)malloc(sizeof(int)*old_cols);
        for (int rank = 1; rank < world_size; rank++) {
            MPI_Recv(next_state, old_cols, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if ( rank > 0 && rank < world_size-1 ) {
                for (int col = 0; col < old_cols; col++) {
                    data[rank-1][col] = next_state[col];
                }
            }
        }
        free(next_state);
        
        printf("rows: %d, columns: %d\n", old_rows, old_cols);
        printf("new states:\n");
        for (int row = 0; row < old_rows; row++) {
            for (int col = 0; col < old_cols; col++) {
                printf("%d, ", data[row][col]);
            }
            printf("\n");
        }
    }
    
    MPI_Send(next_state, old_cols, MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    free2dint(&new_data);
    free(sum_array);
    free(neighbour_array);
    free(pre_sum);
    free(next_sum);
    free(next_state);
    
    MPI_Finalize();
    
    return 0;
}



