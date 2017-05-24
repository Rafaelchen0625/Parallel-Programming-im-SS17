//
//  Measurements.c
//  Assignment 1 for PP
//
//  Created by Raphae Don on 2017/5/4.
//  Copyright © Jahr 2017 Raphae Don. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]){
    
    
    // Initialize the MPI enviroment
    MPI_Init(NULL, NULL);
    
    // Get the number of processors
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get current processor rank
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    
    // Send the whole dataset to other processes
    int N;
    int a[N] = { -5,1,-2,0,1,4,6,-4,3,-6, ...};
    // Broadcasting
    MPI_Bcast(input_message, message_len, MPI_CHAR, 0, MPI_COMM_WORLD);

    
    
    MPI_Status Status;
    int receive_size;
    int* receive_data;
    MPI_Recv(receive_data, receive_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);
    
    // Initialize Parameters
    int max_sum = 0;
    int cur_sum = 0;
    int acc_count = 0;
    int cur_count = 0;
    int end_index = 0;
    
    // Process correspondent sub-dataset
    if ( current_rank == (world_size-1)) { // The last process
        for (int i = (world_size-1)*floor(receive_size/world_size); i < receive_size; i++) {
            cur_sum += receive_data[i];
            ++cur_count;
            // If cur_sum is negativ, resets it and its current accumulation's count
            if (cur_sum < 0) {
                cur_sum = 0;
                cur_count = 0;
            }
            // If cur_sum is greater, record it and its current accumulation's count
            if ( cur_sum > max_sum ) {
                end_index = i;
                acc_count = cur_count;
                max_sum = cur_sum;
            }
        }
    }
    else
    {
        for (int i = current_rank*floor(receive_size/world_size);
             i < (current_rank+1)*floor(receive_size/world_size); i++) {
            cur_sum += receive_data[i];
            ++cur_count;
            // If cur_sum is negativ
            if (cur_sum < 0) {
                cur_sum = 0;
                cur_count = 0;
            }
            
            if ( cur_sum > max_sum ) {
                end_index = i;
                acc_count = cur_count;
                max_sum = cur_sum;
            }
        }
    }
    // Send the sub-max sum and other information to Root, which has responsibility to agglomeration
    // Max_Info contains end_index, acc_count and max_sum
    char* Max_Info;
    MPI_Send(Max_Info, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    // Finalize the MPI enviroment
    
    
    
}
