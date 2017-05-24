//
//  Measurements.c
//  Assignment 1 for PP
//
//  Created by Raphae Don on 2017/5/4.
//  Copyright © Jahr 2017 Raphae Don. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>


int main(int argc, char* argv[]){
    // Initialize the MPI enviroment
    MPI_Init(NULL, NULL);
    
    // Get the number of processors
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get current processor rank
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    
    // Get current name of processor
    char process_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Comm_get_name(MPI_COMM_WORLD, process_name, &name_len);
    
    MPI_Status Status;
    // Process 1 send test_data to process 2
    if ( current_rank == 0 ) {
        // Construct two different size datasets
        int test_data1[100];
        memset(test_data1,0,100);
        double send_time1 = MPI_Wtime();
        MPI_Send(test_data1, 100, MPI_INT, (current_rank+1), 0, MPI_COMM_WORLD);
        // Response from process 2
        MPI_Recv(test_data1, 100, MPI_INT, (current_rank+1), 0, MPI_COMM_WORLD, &Status);
        double receive_time1 = MPI_Wtime();
        
        
        int test_data2[50];
        memset(test_data2, 1, 50);
        double send_time2 = MPI_Wtime();
        MPI_Send(test_data2, 50, MPI_INT, (current_rank+1), 0, MPI_COMM_WORLD);
        // Response from process 2
        MPI_Recv(test_data2, 50, MPI_INT, (current_rank+1), 0, MPI_COMM_WORLD, &Status);
        double receive_time2 = MPI_Wtime();
        
        //time1 = latency + size1 / bandwidth
        double average_time1 = (receive_time1 - send_time1)/2;
        //time2 = latency + size2 / bandwidth
        double average_time2 = (receive_time2 - send_time2)/2;
        
        //bandwidth = (size1-size2)/(time1-time2)
        double bandwidth = (100-50)*sizeof(int)/(average_time1-average_time2);
        //latency = (size1*time2-size2*time1)/(size1-size2)
        double latency = (100*average_time2-50*average_time1 )/(100 - 50);
        
        printf("the latency between notes: %f second, ", latency);
        printf("the bandwidth between notes: %f M/s \n", bandwidth/1000000);
    }
    else if ( current_rank == 1 ){
        // Requist from process 1
        int receive_size = 0;
        MPI_Probe((current_rank-1), 0, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_INT, &receive_size);
        int* receive_data = malloc(receive_size*sizeof(int));
        MPI_Recv(receive_data, receive_size, MPI_INT, (current_rank-1), 0, MPI_COMM_WORLD, &Status);
        // Response to process 1
        MPI_Send(receive_data, receive_size, MPI_INT, (current_rank-1), 0, MPI_COMM_WORLD);
        
        MPI_Probe((current_rank-1), 0, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_INT, &receive_size);
        MPI_Recv(receive_data, receive_size, MPI_INT, (current_rank-1), 0, MPI_COMM_WORLD, &Status);
        // Response to process 1
        MPI_Send(receive_data, receive_size, MPI_INT, (current_rank-1), 0, MPI_COMM_WORLD);
        free(receive_data);
    }
    
    // Finalize the MPI enviroment
    MPI_Finalize();
}
