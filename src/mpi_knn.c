
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>

#include "data_types.h"

void write_knn_output();
void tic();
void toc();

// For time performance evaluation
struct timeval startwtime, endwtime;
double  seq_time;

// Some 
int rank, size;
int D, K;
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
			where K is the KNN num and T is the OpenMP threads_num = 2^T \n");

		return 1;
	}

	K = atoi(argv[1]);
	K++; // The self of each datapoint will be included in the final knn-set.
	printf("K=%d\n",K);

	int THREAD_NUM = 1<<atoi(argv[2]);
  	omp_set_num_threads(THREAD_NUM);
	

	/* Init MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/* Read all Data and Distribute among the rest of the processes*/
	if(rank==0)
	{
		//read_data("./data/formatted_data/mnist_train.txt", &dataSet);
		read_data("./data/formatted_data/mnist_train_svd.txt", &localDataSet);
		//read_data_DUMMY(&localDataSet, 15, 4);
		D = localDataSet.D;
	}

	/* Broadcast Problem Dimensionality of original dataset to all processes */
	MPI_Bcast( &D, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	/*
		Distribute Sub-Datasets to all processes. This has Barrier in it.
	 	NOTE: This is better than MPI_Scatter, cause this is non-blocking, 
		meaning that the indeces-labels and datapoints are all sent in parallel 
	*/
	tic();
	Idistribute_data(&localDataSet, rank, size, D);
	toc();
	printf("[RANK %d]: Data Distribution Time: %f\n",rank, seq_time);
	
	/* As soon as all messages are DONE, we can downsize  the original dataSet of process with rank=0. 
		The case of N/size not being an int is handled by the distribute_data() */
	if(rank==0)
		reallocate_dataset(&localDataSet, (localDataSet.N)/size);

	/* 
		Let the KNN-ing begin...
	*/
	currentDataSet = localDataSet;	
	int p;
	for(p=0; p<size; p++)
	{
		// Communicate next datasets in RING-Topology
		MPI_Request **Rrequests[3];
		MPI_Request **Srequests[3];
		DataSet nextDataSet;
		
		tic();
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
		
		toc();
		printf("[RANK %d]: KNN-Time: %f\n",rank, seq_time);

		// Wait for communication to finish up
		wait_for_request(Rrequests,3);
		wait_for_request(Srequests,3);
		
		toc();
		printf("[RANK %d]: Sub-DataSet Tranfer Time: %f\n",rank, seq_time);

		/* 
		Update Dataset pointers. For the first iterration of the algorithm,
		we must NOT deallocate the currentDataSet, otherwise the localDataSet 
		will be forever lost
		*/
		if(p!=0)
			deallocate_dataset(&currentDataSet);
		currentDataSet = nextDataSet;
	}

	write_knn_output();

	int i;
	for(i=0; i<size; i++)
	{
		if(rank==i)
			print_knn_matrix(&KNN, localDataSet.N, K);
		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 	0;
}


void tic()
{
	gettimeofday (&startwtime, NULL);
}

void toc()
{
	gettimeofday (&endwtime, NULL);
	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
			      + endwtime.tv_sec - startwtime.tv_sec);
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
	dispArray[0] = 0; //Assume zero. CAUSE OFFSET DOESNT BLOODY WORK FOR GCC>4.0.5
	dispArray[1] = 8;

	MPI_Type_struct(2, blockArray, dispArray, array_of_types, &temp);
	MPI_Type_create_resized(temp, dispArray[0], (MPI_Aint) 16, &MPI_KNN_TYPE);
	MPI_Type_commit(&MPI_KNN_TYPE);

	/* 
		Start Writing knn Indeces... The KNN data will be sorted by process.
	*/
	MPI_File fh;
	MPI_File_open(MPI_COMM_WORLD, "knn_indeces", MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
	MPI_File_set_view(fh, 0, MPI_KNN_TYPE, MPI_KNN_TYPE, "native",  MPI_INFO_NULL);

	MPI_Status status;
	int dataLength = (localDataSet.N-1)*K;
	int offset = (dataLength)*rank;
	int c=0,i,j;
	for(i=0;i<localDataSet.N;i++)
		for(j=0;j<K;j++)
		{	
			//KNN[i][j].index = rank*100+c;
			//KNN[i][j].dist = rank*100.0 + c*0.1; For testing purposes
			MPI_File_write_at(fh, offset+c, &(KNN[i][j]), 1, MPI_KNN_TYPE, &status);
			c++;
		}

	MPI_File_close(&fh);
	MPI_Type_free(&MPI_KNN_TYPE);
}