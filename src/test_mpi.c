/**
	This is a test-bed for various MPI utilities 
	
	Author: Savvas Sampaziotis
**/
	
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "data_types.h"

#include <mpi/mpi.h>


struct timeval startwtime, endwtime;


int main(int argc, char** argv){

	/* Init MPI */
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/* Generate Dummy Data*/
	int N=0, D=0;
	DataSet dataSet;
	

	MPI_read_data("./data/formatted_data/mnist_train_svd.txt", &dataSet, rank, size);
	
	
	
	int i;
	for(i=0;i<size;i++)
	{
		if(rank==i)
		{
			printf("RANK %d\n",rank);
			print_dataset(&dataSet);	
			printf("---------\n");
			
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
		


	
	
	MPI_Finalize();
	return 	0;
}

