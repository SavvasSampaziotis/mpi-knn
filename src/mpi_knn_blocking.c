
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>

#include "time_measure.h"
#include "data_types.h"

void write_knn_output();

// For time performance evaluation
TimeInterval knnTime, commTime, knnChunkTime;

// Some 
int rank, size;
int D, K, mod; //mod = the module of N/size. This is nessesery for various operations 
nnPoint** KNN;

// The main datasets that will be used.
DataSet localDataSet;
DataSet currentDataSet;

int main(int argc, char** argv)
{
	/* Algorithm params*/
	if(argc != 3)
	{
		printf("[MPI_KNN]: Error. Number of input args must be 2: ./mpi_knn <K> <T>, \
			where K is the KNN num and T is the OpenMP threads_num\n");
		return 1;
	}

	K = atoi(argv[1]);
	K++; // The self of each datapoint will be included in the final knn-set.
	
	int THREAD_NUM = atoi(argv[2]);
  	omp_set_num_threads(THREAD_NUM);
	
	/* Init MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if(rank==0)
		printf("Size=%d\n", size);

	/* Read all Data and Distribute among the rest of the processes*/
	//mod = MPI_read_data("./data/bin_data/mnist_train_svd.bin", &localDataSet, rank, size);
	//MPI_read_data("./mpi-knn/data/bin_data/mnist_train_svd.bin", &localDataSet, rank, size);
	read_data_DUMMY(&localDataSet, 100, 20);
	mod = 0;

	//MPI_read_data("./data/bin_data/mnist_train.bin", &localDataSet, rank, size);
	D = localDataSet.D;
	
	/* 
		Let the KNN-ing begin...
	*/
	MPI_Barrier(MPI_COMM_WORLD);
	tic(&knnTime);

	currentDataSet = localDataSet;	
	int p;
	for(p=0; p<size; p++)
	{
		// Communicate next datasets in RING-Topology
		MPI_Request **Srequests[2]; // not really needed...
		DataSet nextDataSet;
		
		tic(&commTime);

		if(size > 1)
			if(rank == 0)
			{
				Isend_dataset(&currentDataSet, rank+1, Srequests); 
				receive_dataset(&nextDataSet, size-1, D);	
			}
			else if(rank == size-1)
			{
				Isend_dataset( &currentDataSet,  0, Srequests);
				receive_dataset(&nextDataSet, rank-1, D); // Blocking
			}
			else
			{
				Isend_dataset( &currentDataSet,  rank+1, Srequests);
				receive_dataset(&nextDataSet, rank-1, D);
			}	
		toc(&commTime);	
		/*
			Do thuh KNN thing, while nextDataSet is coming right up.
		*/
		tic(&knnChunkTime);
		if(p==0) // First Iteration
			knn( &localDataSet, &currentDataSet, K, &KNN );
		else
			update_knn( &localDataSet, &currentDataSet, K, &KNN );
		toc(&knnChunkTime);
		
		/* 
		Update Dataset pointers. For the first iterration of the algorithm,
		we must NOT deallocate the currentDataSet, otherwise the localDataSet 
		will be forever lost
		*/
		if(p!=0)
			deallocate_dataset(&currentDataSet);
		currentDataSet = nextDataSet;

		printf("[RANK %d]: Comm Time = %lf KNN chunk Time = %lf\n",rank, commTime.seqTime, knnChunkTime.seqTime);
	}
	

	MPI_Barrier(MPI_COMM_WORLD);
	toc(&knnTime);
	if(rank==0)
		printf("knn-time = %lf\n",rank, knnTime.seqTime);

	//write_knn_output();
	
	MPI_Finalize();
	return 	0;
}



/**
	This function writes the KNN utilising the MPI File I/O. 
*/
void write_knn_output()
{
	/* 
		Declare special MPI datatype for the nnPoint struct
		nnPoint = [int, 4-byte-hole, double];
		MPI_KNN_TYPE = [MPI_INT,MPI_DOUBLE];
	*/
	MPI_Datatype temp, MPI_KNN_TYPE;
	int blockArray[2]= {1,1};
	MPI_Aint dispArray[2];
	MPI_Datatype array_of_types[2] = {MPI_INT, MPI_DOUBLE};
	//dispArray[0] =  (MPI_Aint)offsetof( nnPoint, index);
	//dispArray[1] =  (MPI_Aint)offsetof( nnPoint, dist); This doesnt compile in Hellasgrid
	dispArray[0] = 0; //Assume zero. CAUSE OFFSET DOESNT BLOODY WORK FOR GCC>4.0.5 in HG
	dispArray[1] = 8;

	MPI_Type_struct(2, blockArray, dispArray, array_of_types, &temp);
	MPI_Type_create_resized(temp, dispArray[0], (MPI_Aint) 16, &MPI_KNN_TYPE);
	MPI_Type_commit(&MPI_KNN_TYPE);

	/* 
		Start Writing knn Indeces... The KNN data will be sorted by process.
	*/
	MPI_File fh;
	MPI_File_open(MPI_COMM_WORLD, "mpi_knn_output", MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
	if(fh<=0)
		printf("[RANK %d]: Error Opeining Output file", rank);

	MPI_File_set_view(fh, 0, MPI_KNN_TYPE, MPI_KNN_TYPE, "native",  MPI_INFO_NULL);

	MPI_Status status;
	int dataLength = (localDataSet.N)*(K-1);
	int offset;
	if(rank==size-1)
		offset = (localDataSet.N - mod)*(K-1)*rank;
	else
		offset = (localDataSet.N)*(K-1)*rank;

	int c=0,i,j;
	for(i=0;i<localDataSet.N;i++)
		for(j=1;j<K;j++)
		{	
			//KNN[i][j].dist = 10;
			//KNN[i][j].index = rank*100+i*10+j;
			MPI_File_write_at(fh, offset+c, &(KNN[i][j]), 1, MPI_KNN_TYPE, &status);
			c++;
		}

	MPI_File_close(&fh);
	MPI_Type_free(&MPI_KNN_TYPE);

}


	/*
	int i;
	for(i=0; i<size; i++)
	{
		if(rank==i)
			print_knn_matrix(&KNN, localDataSet.N, K);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	
	for(i=0;i<size;i++)
	{
		if(rank==i)
		{
			printf("RANK %d\n",rank);
			print_dataset(&localDataSet);	
			printf("---------\n");
			
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
		*/