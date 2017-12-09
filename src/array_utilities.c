
#include "stdio.h"
#include <stdlib.h>

#include "data_types.h"


#include <string.h>

void allocateArray(double** A, int N, int D)
{
	int i;
	
	A = (double **) malloc(N*sizeof(double*));
	
	for (i = 0; i<N; i++){
		//double* temp = (double *) malloc(D*sizeof(double));
		(A[i]) = (double *) malloc(D*sizeof(double));
	}	
}

/**
	Reads data from file...
*/
void readData(dataPoint **dataSet, int N, int D){
	int i,j;
	
	*dataSet = (dataPoint*) malloc(N*sizeof(dataPoint));
	
	for(i=0; i<N; i++){

		(*dataSet)[i].label = i;
		(*dataSet)[i].point = (double*) malloc(D*sizeof(double));
		for(j=0; j<D; j++)
			(*dataSet)[i].point[j] = i+j;
	}
}


void printDataPoint(dataPoint dp, int D){
	int i;
	for (i = 0; i < D; ++i)
		printf("%f ",dp.point[i]);	
	printf("\t%d\n", dp.label);
}


/**
	Prints array of NxD elements 
*/
void printDataset(dataPoint* dataSet, int N, int D){
	int i;
	for (i = 0; i<N; i++){
		printDataPoint(dataSet[i],D);
		printf("\n");
	}
}



void printArray(double** A, int N, int M){
	int i,j;
	for (i = 0; i<N; i++){
		for (j = 0; j<M; j++)
			printf("%f ",A[i][j]);
		printf("\n");
	}
}

