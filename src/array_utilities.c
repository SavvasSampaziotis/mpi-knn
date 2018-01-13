
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_types.h"

void allocate_empty_dataset(DataSet* dataSet, int N, int D);
void read_data(const char* filename, DataSet *dataSet);
void read_data_DUMMY(DataSet *dataSet, int N, int D);
void deallocate_dataset(DataSet* dataSet);
void reallocate_dataset(DataSet* dataSet, int newN);
void print_dataset(DataSet* dataSet);
void print_array(double** A, int N, int M);
void print_knn_matrix(nnPoint*** KNN, int N, int K);
void mergesort_nnpoint_arrays(nnPoint **A, nnPoint **B, int K, /*out*/ nnPoint** C);


/**
	Reads data from file...
*/
void read_data_DUMMY(DataSet *dataSet, int N, int D)
{
	int i,j;
	
	allocate_empty_dataset(dataSet,N,D);
	
	for(i=0; i<N; i++)
	{
		dataSet->index[i] = i;
		dataSet->label[i] = i;
		for(j=0; j<D; j++)
			dataSet->dataPoints[i][j] = i+j;
	}
}


void allocate_empty_dataset(DataSet* dataSet, int N, int D){
	int i,j;
	
	dataSet->data = (double*) malloc(N*D*sizeof(double));
	dataSet->N = N;
	dataSet->D = D;
	dataSet->dataPoints = (double**) malloc(N*sizeof(double*));

	dataSet->index = (int*) malloc(N*sizeof(int));
	dataSet->label = (int*) malloc(N*sizeof(int));

	for(i=0; i<N; i++)
	{
		dataSet->index[i] = -1;
		dataSet->label[i] = -1;

		// Each dataPoint points to a row of the whole data matrix
		dataSet->dataPoints[i] = &(dataSet->data[D*i]);
		for(j=0; j<D; j++)
			dataSet->dataPoints[i][j] = 11112; // For debbuging purposes.
	}

}


void deallocate_dataset(DataSet* dataSet)
{	
	int i;

	// All Datapoint structs is now freed
	free(dataSet->dataPoints);
	
	// All data are now freed. Data has size [1,N*D], so one 'free()' will suffice.
	free(dataSet->data);
}

/**
	Resizes the dataSet's allocated memory. 

	The address and size of the actual struct remains unchanged. 

	int start = i-th datapoint that will be the first entry to the new data set.  
*/
void reallocate_dataset(DataSet* dataSet, int newN)
{
	int D = dataSet->D;
	dataSet->N = newN;

	dataSet->data = (double*) realloc(dataSet->data, newN*D*sizeof(double));

	dataSet->dataPoints = (double**) realloc( dataSet->dataPoints, newN*sizeof(double*) );
	dataSet->index = (int*) realloc(dataSet->index, newN*sizeof(int));
	dataSet->label = (int*) realloc(dataSet->label, newN*sizeof(int));

	// re init the pointer of each datapoint, in case realloc() 
	// moved the content to another memory location
	int i;
	for(i=0; i<newN; i++)
		dataSet->dataPoints[i] = &(dataSet->data[D*i]);
}


/**
	Prints dataset-struct. 
*/
void print_dataset(DataSet* dataSet)
{
	int i,j;
	for (i = 0; i<dataSet->N; i++)
	{
		printf("\t%d: [", dataSet->index[i]);
		for (j = 0; j < dataSet->D; j++)
		{
			printf("%lf ",dataSet->dataPoints[i][j]);	
		}
		printf("]\t%d\n", dataSet->label[i]);
	}
}


void print_array(double** A, int N, int M)
{
	int i,j;
	for (i = 0; i<N; i++){
		for (j = 0; j<M; j++)
			printf("%f ",(A)[i][j]);
		printf("\n");
	}
}

void print_knn_matrix(nnPoint*** KNN, int N, int K)
{
	int i;
	for (i = 0; i<N; i++){
		printf("\n");
		int k;

		for(k=0; k<K; k++)
		{
			printf("[%d %lf]\t", (*KNN)[i][k].index, (*KNN)[i][k].dist);
		}

		printf("\n");
	}
}


/**
	This function merges 2 nnPoint arrays of length-K, and returns a K-length sorted nnPoint array.  
	WARNING: It is assumed that input arrays are already sorted.
**/
void mergesort_nnpoint_arrays(nnPoint **A, nnPoint **B, int K, /*out*/ nnPoint** C)
{	
	*C = malloc(K*sizeof(nnPoint));
	int k, a=0,b=0;
	for(k=0;k<K;k++)
	{	
		if( (*A)[a].dist < (*B)[b].dist)
		{
			(*C)[k] = (*A)[a]; // This is a Struct copy operation. Basically memcpy...
			a++;
		}
		else
		{
			(*C)[k] = (*B)[b]; // This is a Struct copy operation. Basically memcpy...
			b++;
		}
	}	
}

