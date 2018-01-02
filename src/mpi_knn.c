
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi/mpi.h>

#include "data_types.h"


struct timeval startwtime, endwtime;
double  seq_time;

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
		read_data_DUMMY(&dataSet, 6000000, 4);
		N = dataSet.N;
		D = dataSet.D;
	}

	//Broadcast Size of original dataset to all processes
	MPI_Bcast( &N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast( &D, 1, MPI_INT, 0, MPI_COMM_WORLD);

	
	/*This has Barrier in it*/
	gettimeofday (&startwtime, NULL);
	{
		Idistribute_data(&dataSet, rank, size);
	}
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("[RANK %d]: Data Distribution Time: %f\n",rank, seq_time);
	
	/*
	As soon as all messages are DONE, we can downsize 
		the original dataSet of prosses rank=0. 
		The case of N/size not being an int is handled by the distribute_data()
	*/
	if(rank==0)
		reallocate_dataset(&dataSet, (dataSet.N)/size);

	/*if(rank==2)
		print_dataset(&dataSet);*/
	

	//Begin KNN-ing
	int p;
	/*
	// Communicate next datasets in RANK-Topology
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

