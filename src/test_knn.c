
#include <stdlib.h>
#include "stdio.h"



int main(int argc, char** argv){
	
	int N,D;	
	double** dataSet;

	N = 11;
	D = 4;

	readData(dataSet, N, D);

	printf("%f\n", dataSet[1][1] );

	printArray(dataSet, N, D);
	
	
	int K=2, i;
	double** KNN = (double**) malloc(N*sizeof(double*));
	for(i=0; i<K; i++)
		KNN[i] = (double*) malloc(D*sizeof(double*)); 

	

	double **temp;

	knn(dataSet,N,D, K,KNN,temp);

	

	//printArray(KNN, N, K);

	return 0;
}


