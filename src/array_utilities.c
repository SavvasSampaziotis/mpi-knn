
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
	int i;
	
	*dataSet = (dataPoint*) malloc(N*sizeof(dataPoint));
	
	for(i=0; i<N; i++){
		/*dataPoint temp;
		temp.label = i;
		temp.point[0] = i;
		temp.point[1] = i+1;*/
		//memcpy( &(dataSet[i]), &temp, sizeof(temp) );
		//dataSet[i].dataPointnt[i] = (double*) malloc(D*sizeof(double));


		(*dataSet)[i].label = i;
		(*dataSet)[i].point[0]= i;
		(*dataSet)[i].point[1]= i+1;
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
void printArray(dataPoint* dataSet, int N, int D){
	int i,j;
	for (i = 0; i<N; i++){
		printDataPoint(dataSet[i],D);
		printf("\n");
	}
}

