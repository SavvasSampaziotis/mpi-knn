/**
 knn.c

 The knn library that handles the parallel
  or non parallel executio of the knn algorithm on a block of data

	Author: Savvas Sampaziotis
*/

#include "omp.h"

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "data_types.h"

double calc_dist(DataPoint A, DataPoint B, int D);
void distance_matrix_SEQ(DataSet *dataSet, nnPoint*** distMatrix);
void distance_matrix_OMP(DataSet *dataSet, nnPoint*** distMatrix);
int cmpfunc (const void * a, const void * b);

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
void knn(DataSet *dataSet, int K, nnPoint*** KNN )
{
	int N = dataSet->N;
	int D = dataSet->D;
	DataPoint* dataPoints = dataSet->dataPoints;


	// N-Array of knns' for each element. size(KNN)=[N,K]
	*KNN = (nnPoint**) malloc(N*sizeof(nnPoint*));

	// Calculate the Distance Matrix. 
	nnPoint ** distMatrix;
	distance_matrix_OMP( dataSet, &distMatrix);

	int i;
	for(i=0; i<N; i++)
		(*KNN)[i] = (nnPoint*) malloc(K*sizeof(nnPoint)); 	

	#pragma omp parallel for schedule(static) num_threads(4)
		for(i=0; i<N; i++)
		{
			 qsort(distMatrix[i], N, sizeof(nnPoint), cmpfunc);
			 int j;
			 for(j=0; j<K; j++){ // Disregard first element
			 	(*KNN)[i][j].dist = distMatrix[i][j+1].dist;
			 	(*KNN)[i][j].dpoint = distMatrix[i][j+1].dpoint;
			 }
			 	
		}

	// Free distance matrix, AFTER we are done with the 
	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);
}  

/**
	distance_matrix calculates all distances between all datapoints and stores 
	them in matric distMatrix [N,N] in an efficient way.

	distMatrix is symmetric with its diagonal==0. Thus, the calculation is 
	done efficiently, by performing N(N-1)/2 calc_dist calls instead of N^2.  
*/
void distance_matrix_SEQ(DataSet *dataSet, nnPoint*** distMatrix)
{		
	int N = dataSet->N;
	int D = dataSet->D;
	DataPoint* dataPoints = dataSet->dataPoints;

	printf("%d\n", N);
	printDataPoint(dataPoints[4], D);

	*distMatrix = (nnPoint**) malloc(N*sizeof(nnPoint*));		
	int i;
	for(i=0; i<N; i++)
		(*distMatrix)[i] = (nnPoint*) malloc(N*sizeof(nnPoint));

	for(i=0; i<N; i++)
	{
		(*distMatrix)[i][i].dist = 0;
		(*distMatrix)[i][i].dpoint = &( dataPoints[i] );

		int j;
		for(j=i+1; j<N; j++)
		{
			double d = calc_dist( dataPoints[i], dataPoints[j], D);
			(*distMatrix)[i][j].dist = d;
			(*distMatrix)[i][j].dpoint = &( dataPoints[j] );
			
			(*distMatrix)[j][i].dist = d;
			(*distMatrix)[j][i].dpoint = &( dataPoints[i] );
		}
	}
}

/**
	distance_matrix_OMP calculates all distances between all datapoints and stores 
	them in matric distMatrix [N,N] in an efficientand way as well as in parallel.

	distMatrix is symmetric with its diagonal==0. Thus, the calculation is 
	done efficiently, by performing N(N-1)/2 calc_dist calls instead of N^2.

	Moreover each row of the matric is calculated in parallel (chunks). 
	The workload of each OpenMP thread is such that the total load is constant.
	 The 0th line is paired with the Nth, the 1st with N-1th an so on.
	 This is due to the symmetrical-calculation of the matrix. 

*/
void distance_matrix_OMP(DataSet *dataSet,  nnPoint*** distMatrix)
{
	omp_set_num_threads(4);

	int N = dataSet->N;
	int D = dataSet->D;
	DataPoint* dataPoints = dataSet->dataPoints;

	*distMatrix = (nnPoint**) malloc(N*sizeof(nnPoint*));	
	
	int i;
	for(i=0; i<N; i++)
		(*distMatrix)[i] = (nnPoint*) malloc(N*sizeof(nnPoint));
	
	/* This for-loop pairs the nested loops into workload of equal size.
		 	1st j-loop with Nth loop, 2nd with (N-1)th etc.
		 	That way eac thread performs N calculation in total.  */
	#pragma omp parallel for schedule(static) 
		for(i=0; i<N/2; i++)
		{
			//This fills up the diagonal
			(*distMatrix)[i][i].dist = 0;
			(*distMatrix)[i][i].dpoint = &( dataPoints[i] );

			(*distMatrix)[N-i-1][N-i-1].dist = 0;
			(*distMatrix)[N-i-1][N-i-1].dpoint = &( dataPoints[N-i-1] );

			int j;
			for(j=i+1; j<N; j++)
			{
				double d = calc_dist(dataPoints[i], dataPoints[j], D);
				(*distMatrix)[i][j].dist = d;
				(*distMatrix)[i][j].dpoint = &( dataPoints[j] );
				
				(*distMatrix)[j][i].dist = d;
				(*distMatrix)[j][i].dpoint = &( dataPoints[i] );
			}
			for(j=N-i-1; j<N; j++){
				double d = calc_dist(dataPoints[N-i-1], dataPoints[j], D);
				(*distMatrix)[N-i-1][j].dist = d;
				(*distMatrix)[N-i-1][j].dpoint = &( dataPoints[j] );
				
				(*distMatrix)[j][N-i-1].dist = d;
				(*distMatrix)[j][N-i-1].dpoint = &( dataPoints[N-i-1] );
			}
		}
}


/**
	Calculates distance between datapoint A and B. 
	Each of these are double arrays of length D.
*/
double calc_dist(DataPoint A, DataPoint B, int D){
	
	int i;
	double temp, dist = 0;
	for(i=0; i<D; i++){
		temp = A.point[i]-B.point[i]; 
		dist += temp*temp;
	}
	//printf("Dist = %f\n", dist);
	return dist;
}

/**
	Used for qsort. Compares two nnPoint, by distance
*/
int cmpfunc (const void * a, const void * b) {
   nnPoint* p1 = (nnPoint*) a;
   nnPoint* p2 = (nnPoint*) b;

   return p1->dist - p2->dist;
}
