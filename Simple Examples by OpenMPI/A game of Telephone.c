//
//  main.c
//  A game of Telephone
//
//  Created by Raphae Don on 2017/5/2.
//  Copyright © Jahr 2017 Raphae Don. All rights reserved.
//
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    if (argc != 2){
        printf("please input a valid string to pass down!");
    }
    
    int message_size = strlen(argv[1]);
    
    // Initialize the MPI enviroment
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int process_size;
    MPI_Comm_size(MPI_COMM_WORLD, &process_size);
    
    // Get the current rank of the process
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    
    //Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Process Message
    MPI_Status Status;
    int receive_size = 0;
    if (current_rank == 0) {
        MPI_Send(argv[1], message_size, MPI_CHAR, (current_rank+1), 0, MPI_COMM_WORLD);
    }
    else if (current_rank < process_size-1) {
        MPI_Probe((current_rank-1), 0, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_CHAR, &receive_size);
        char* Message = malloc(receive_size*sizeof(MPI_CHAR));
        MPI_Recv(Message, receive_size, MPI_CHAR, (current_rank-1), 0, MPI_COMM_WORLD, &Status);
        MPI_Send(Message, receive_size, MPI_CHAR, (current_rank+1), 0, MPI_COMM_WORLD);
        free(Message);
    }
    else {
        MPI_Probe((current_rank-1), 0, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_CHAR, &receive_size);
        char* Message = malloc(receive_size*sizeof(MPI_CHAR));
        MPI_Recv(Message, receive_size, MPI_CHAR, (current_rank-1), 0, MPI_COMM_WORLD, &Status);
        //Print off a hello word message
        printf("Message end here: processor %s, rank %d \n"
               "Message ist: %s \n",
               processor_name, current_rank, Message);
        free(Message);
    }
    
    //Finalize the MPI enviroment
    MPI_Finalize();
}
