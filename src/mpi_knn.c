
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi/mpi.h>

#include "data_types.h"


struct timeval startwtime, endwtime;

int main(int argc, char** argv){

	/* Init MPI */
	int rank, size, P;
	int N,D;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	DataSet dataSet;
	MPI_Request requests[3];
	
	if(rank==0)
	{
		/* Read all Data and Distribute among the rest of the processes*/
		//read_data("./data/formatted_data/mnist_train.txt", &dataSet);
		read_data_DUMMY(&dataSet, 20, 3);
		N = dataSet.N;
		D = dataSet.D;
	}
	//Broadcast Size of original dataset to all processes

	MPI_Bcast( &N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast( &D, 1, MPI_INT, 0, MPI_COMM_WORLD);
	

	if(rank==0)
	{	//WARNING: This will NOT be blocked. 
		distribute_data(&dataSet, size);
		printf("DONE\n");
	}
	else
	{	
		receive_dataset(&dataSet, 0);
		printf("RANK %d\n", rank);
	}
	
	/*
	As soon as all messages are DONE, we can downsize 
		the original dataSet of prosses rank=0. 
		The case of N/size not being an int is handled by the distribute_data()
	*/ 
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	if(rank==0)
		reallocate_dataset(&dataSet, (dataSet.N)/size);
//
	print_dataset(&dataSet);
	




	//Begin KNN-ing
	/*
	// Communicate nex datasets in RANK-Tpology
	DataSet nextDataSet;
	if(rank == 0)
	{
		receive_dataset(&nextDataSet, size-1);
		send_dataset( &dataSet,  rank+1);
	}
	else if(rank == size-1)
	{
		receive_dataset(&nextDataSet, rank-1);
		send_dataset( &dataSet,  0);
	}
	else
	{
		receive_dataset(&nextDataSet, rank-1);
		send_dataset( &dataSet,  rank+1);
	}
	*/

		
	MPI_Finalize();
	return 	0;
}

