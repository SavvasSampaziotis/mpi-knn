
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
	

	//read_data_MPI("./data/formatted_data/mnist_train_svd.txt", &dataSet, rank, size);
	MPI_File fh;	
	MPI_File_open(	MPI_COMM_WORLD, "./data/formatted_data/mnist_train_svd.txt",\
	 				MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

	
	// Read Header
	int buff[2];
	MPI_Status status;
	
	MPI_Datatype headerType;
	int hLengths[2] = {1,1};
	MPI_Aint hDisps[2] = {0,3*8};
	MPI_Datatype hTypes[2] = {MPI_INT, MPI_INT};
	MPI_Type_struct(2, hLengths, hDisps, hTypes, &headerType );	
	MPI_Type_commit(&headerType);
	
	MPI_File_set_view(fh, 0, MPI_INT, headerType, "native", MPI_INFO_NULL);

	MPI_File_seek(fh, 0, MPI_SEEK_SET); // Move cursor 1 byte
	MPI_File_read(fh, &N, 1, MPI_INT, &status);
	MPI_File_seek(fh, 1, MPI_SEEK_CUR); // Move cursor 1 byte
	MPI_File_read(fh, &D, 1, MPI_INT, &status);

	//MPI_File_read(fh, &N, 1, MPI_INT, &status);
	//MPI_File_read(fh, &D, 1, MPI_INT, &status);
	
	//MPI_File_set_view(fh, 1, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
	printf("N=%d \t D=%d \n", N,D);
	
	MPI_Type_free(&headerType);
	// Read Body of data-file
	
	MPI_File_close(&fh);


	MPI_Finalize();
	return 	0;
}

