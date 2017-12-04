
#include "stdio.h"
#include <stdlib.h>




void allocateArray(double** A, int N, int D){
	int i;
	
	A = (double **) malloc(N*sizeof(double*));
	for (i = 0; i<N; i++)
		A[i] = (double *) malloc(D*sizeof(double));
}

/**
	Reads data from file...
*/
void readData(double** dataSet, int N, int D){
	int i,j;
	int k=0;

	
	allocateArray(dataSet, N,D);
	
	for (i = 0; i<N; i++)
		for(j=0; j<D; j++){
			//a[i][j] = rand() % N;	
			dataSet[i][j] = k;
			k++;
		}
	
}

/**
	Prints array of NxD elements 
*/
void printArray(double** A, int N, int D){
	int i,j;
	for (i = 0; i<N; i++){
		for(j=0; j<D; j++){
			printf("%f ", A[i][j]);
		}
		printf("\n");
	}

}

