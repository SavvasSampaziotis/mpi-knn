
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi/mpi.h>

#include "data_types.h"


struct timeval startwtime, endwtime;

int main(int argc, char** argv){

	/* Init MPI */
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if(rank==0)
	{
		/* Read all Data and Distribute among the rest of the processes*/
		DataSet dataSet;
		//read_data("./data/formatted_data/mnist_train.txt", &dataSet);
		read_data_DUMMY(&dataSet, 20, 3);
		
		//print_dataset(&dataSet);	
		distribute_data(&dataSet, size);

		reallocate_dataset(&dataSet, 3);
		//print_dataset(&dataSet);	
	}
	else
	{
		DataSet recvDataSet;
		receive_dataset(&recvDataSet);

		//printf("RANK: %d\tRecieving Dataset...\n", rank);
		//print_dataset(&recvDataSet);
	}
	
	//As soon as the last message is DONE, 
	MPI_Barrier(MPI_COMM_WORLD);


		
	MPI_Finalize();
	return 	0;
}

