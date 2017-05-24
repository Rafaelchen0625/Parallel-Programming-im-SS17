//
//  main.c
//  Hello World
//
//  Created by Raphae Don on 2017/5/2.
//  Copyright © Jahr 2017 Raphae Don. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>

int main(int argc, const char * argv[]) {
    // initialize the MPI environment
    MPI_Init(NULL, NULL);
    
    //Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    //Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    //Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    //Print off a hello word message
    printf("Hello, World from processor %s, rank %d"
           " out of %d processors\n",
           processor_name, world_rank, world_size);
    
    //Finalize the MPI enviroment.
    MPI_Finalize();
}
