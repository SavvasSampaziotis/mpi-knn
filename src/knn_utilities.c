/**
 knn.c

 The knn library that handles the parallel
  or non parallel executio of the knn algorithm on a block of data

	Author: Savvas Sampaziotis
*/

#include "omp.h"

#include "stdio.h"
#include <stdlib.h>

#include "data_types.h"

double calc_dist(dataPoint A, dataPoint B, int D);
void distance_matrix(dataPoint **dataSet, int N, int D, double*** distMatrix);
void distance_matrix_OMP(dataPoint **dataSet, int N, int D, double*** distMatrix);


/**
	The knn function calculates the K nearest neighbors for each element in array A.
		double** A = NxD matrix with data points
		int N = number of data points in A
		int D = number of dimensions of each data points
		int K = K-NN parameters

	return argument:
		int** KNN = [N,K]. Each row contains K indeces representing the K-NNs 
			for each  data-point.
		double** KNN_dist =[N,K]. Likewise, this matrix contains the calculated distance.
*/
void knn(dataPoint **dataSet, int N, int D, int K, nnPoint*** KNN ){
	
	// N-Array of knns' for each element. size(KNN)=[N,K]
	*KNN = (nnPoint**) malloc(N*sizeof(nnPoint*));

	// Calculate the Distance Matrix. 
	double ** distMatrix;
	distance_matrix(dataSet, N,D, &distMatrix);



	int i;
	for(i=0; i<N; i++){
		(*KNN)[i] = (nnPoint*) malloc(K*sizeof(nnPoint));

	}

	// Free distance matrix
	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);
}  

/**
	knn_searc performs the distance calculation for
*/
void distance_matrix(dataPoint **dataSet, int N, int D, double*** distMatrix)
{		
	*distMatrix = (double**) malloc(N*sizeof(double*));		
	int i;
	for(i=0; i<N; i++)
		(*distMatrix)[i] = (double*) malloc(N*sizeof(double));
	for(i=0; i<N; i++)
	{
		(*distMatrix)[i][i] = 0;
		int j;
		for(j=i+1; j<N; j++)
		{
			double d = calc_dist((*dataSet)[i],(*dataSet)[j], D);
			(*distMatrix)[i][j] = d;
			(*distMatrix)[j][i] = d;
		}
	}
}

/**
	knn_searc performs the distance calculation for
*/
void distance_matrix_OMP(dataPoint **dataSet, int N, int D, double*** distMatrix)
{
	omp_set_num_threads(4);

	*distMatrix = (double**) malloc(N*sizeof(double*));	
	
	int i;
	for(i=0; i<N; i++)
		(*distMatrix)[i] = (double*) malloc(N*sizeof(double));
	
	/* This for-loop pairs the nested loops into workload of equal size.
		 	1st j-loop with Nth loop, 2nd with (N-1)th etc.
		 	That way eac thread performs N calculation in total.  */
	#pragma omp parallel for schedule(static) 
		for(i=0; i<N/2; i++)
		{
			(*distMatrix)[i][i] = 0;
			(*distMatrix)[N-i-1][N-i-1] = 0;

			int j;
			for(j=i+1; j<N; j++)
			{
				double d = calc_dist((*dataSet)[i],(*dataSet)[j], D);
				(*distMatrix)[i][j] = d;
				(*distMatrix)[j][i] = d;
			}
			for(j=N-i-1; j<N; j++){
				double d = calc_dist((*dataSet)[N-i-1],(*dataSet)[j], D);
				(*distMatrix)[N-i-1][j] = d;
				(*distMatrix)[j][N-i-1] = d;
			}
		}
}
/**
	Calculates distance between datapoint A and B. 
	Each of these are double arrays of length D.
*/
double calc_dist(dataPoint A, dataPoint B, int D){
	
	int i;
	double temp, dist = 0;
	for(i=0; i<D; i++){
		temp = A.point[i]-B.point[i]; 
		dist += temp*temp;
	}
	//printf("Dist = %f\n", dist);
	return dist;
}




void test(int* a, int N) {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }
  printf(" TEST %s\n", (pass) ? "PASSed" : "FAILed");
}