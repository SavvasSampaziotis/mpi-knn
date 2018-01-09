
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
	int D;
	int K = 3;
	nnPoint** KNN;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	DataSet localDataSet;
	DataSet currentDataSet;
	
	
	
	/* Read all Data and Distribute among the rest of the processes*/
	if(rank==0)
	{
		//read_data("./data/formatted_data/mnist_train.txt", &dataSet);
		read_data_MPI("./data/formatted_data/mnist_train_svd.txt", &localDataSet);

		read_data_DUMMY(&localDataSet, 15, 4);
		D = localDataSet.D;

	}

	/* Broadcast Problem Dimensionality of original dataset to all processes */
	MPI_Bcast( &D, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	/* Distribute Sub-Datasets to all processes. This has Barrier in it */
	gettimeofday (&startwtime, NULL);
	{
		Idistribute_data(&localDataSet, rank, size, D);
	}
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("[RANK %d]: Data Distribution Time: %f\n",rank, seq_time);
	
	/* As soon as all messages are DONE, we can downsize  the original dataSet of process with rank=0. 
		The case of N/size not being an int is handled by the distribute_data() */
	if(rank==0)
		reallocate_dataset(&localDataSet, (localDataSet.N)/size);




	currentDataSet = localDataSet;	

	//Begin KNN-ing
	int p;
	for(p=0; p<size; p++)
	{
		// Communicate next datasets in RING-Topology
		MPI_Request **Rrequests[3];
		MPI_Request **Srequests[3];
		DataSet nextDataSet;
		
		/*if(rank==0)
		{	
			print_dataset(&localDataSet);
			printf("--\n");
			print_dataset(&currentDataSet);
			printf("\n-------------------\n");
		}*/
		gettimeofday (&startwtime, NULL);
		if(rank == 0)
		{
			Isend_dataset(&currentDataSet, rank+1, Srequests);
			Ireceive_dataset(&nextDataSet, size-1, D, Rrequests);	
		}
		else if(rank == size-1)
		{
			Isend_dataset( &currentDataSet,  0, Srequests);
			Ireceive_dataset(&nextDataSet, rank-1, D,  Rrequests);
		}
		else
		{
			Isend_dataset( &currentDataSet,  rank+1, Srequests);
			Ireceive_dataset(&nextDataSet, rank-1, D, Rrequests);
		}	
		
		/*
			Do thuh KNN thing, while nextDataSet is coming right up.
		*/

		
		
		if(p==0) // First Iteration
			knn( &localDataSet, &currentDataSet, K, &KNN );
		else
			update_knn( &localDataSet, &currentDataSet, K, &KNN );
		

		wait_for_request(Rrequests,3);
		wait_for_request(Srequests,3);
		
		gettimeofday (&endwtime, NULL);
	 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
			      + endwtime.tv_sec - startwtime.tv_sec);
		printf("[RANK %d]: Sub-DataSet Tranfer Time: %f\n",rank, seq_time);

		/* Update Dataset pointers. For the first iterration of the algorithm,
		 we must NOT deallocate the currentDataSet, otherwise the localDataSet 
		 will be forever lost */
		if(p!=0)
			deallocate_dataset(&currentDataSet);
		currentDataSet = nextDataSet;
	}



	//print_knn_matrix(&tempKNN, localDataSet.N, K );
	
	
	MPI_Finalize();
	return 	0;
}

