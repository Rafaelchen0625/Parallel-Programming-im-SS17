//
//  main.c
//  Assignment 1 for PP
//
//  Created by Raphae Don on 2017/5/2.
//  Copyright © Jahr 2017 Raphae Don. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        printf("bitte geben Sie richtige(s) Wort(e) ein!");
    }
    
    int message_len = strlen(argv[1]);
    char* input_message = malloc(message_len*sizeof(char));
    strncpy(input_message, argv[1], message_len);
    
    // Initialize the MPI enviroment
    MPI_Init(NULL, NULL);
    
    // Get number of processes
    int processes_size;
    MPI_Comm_size(MPI_COMM_WORLD, &processes_size);
    
    // Get current rank of process
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    
    // Get current name of process
    char processor_names[MPI_MAX_PROCESSOR_NAME];
    int name_num;
    MPI_Comm_get_name(MPI_COMM_WORLD, processor_names, &name_num);
    
    // Broadcasting
    MPI_Bcast(input_message, message_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    printf("In %s processor: %d...\n",processor_names,current_rank);
    printf("Message is %s\n",input_message);

    // Finalize the MPI enviroment
    MPI_Finalize();
}
