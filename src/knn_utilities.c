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

double calc_dist(double* A, double* B, int D);
void distance_matrix_SEQ(DataSet *localDataSet, DataSet *givenDataSet,  nnPoint*** distMatrix);
void distance_matrix_OMP(DataSet *localDataSet, DataSet *givenDataSet,  nnPoint*** distMatrix);
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
void knn(DataSet *localDataSet, DataSet *inputDataSet, int K, nnPoint*** KNN )
{
	int N = localDataSet->N;
	int N2 = inputDataSet->N; // Only used in step 4
	int D = localDataSet->D;
	
	/* Step 1: Init N-Array of knns' for each element. size(KNN)=[N,K]*/
	*KNN = (nnPoint**) malloc(N*sizeof(nnPoint*));

	/* Step 2: Calculate the Distance Matrix. */
	nnPoint ** distMatrix;
	distance_matrix_OMP( localDataSet, inputDataSet, &distMatrix);

	int i;
	for(i=0; i<N; i++)
		(*KNN)[i] = (nnPoint*) malloc(K*sizeof(nnPoint)); 	

	/* Step 3: Sort each row of the Distance Matrix. */
	#pragma omp parallel for schedule(static) num_threads(4)
		for(i=0; i<N; i++)
		{
			 qsort(distMatrix[i], inputDataSet->N, sizeof(nnPoint), cmpfunc);
			 int j, offset=0;

			/* Step 4: Store the K-first elements of the sorted DistMatrix. These are the Knn's! */

			/* Sometimes, the closest neighbor happens to be the input Datapoint itself.  
				Omit the first element in case it has the same index. */			 
		 	if(distMatrix[i][0].index == localDataSet->index[i])
		 		offset = 1;

			for(j=0; j<K; j++)
			{
				(*KNN)[i][j].dist = distMatrix[i][j+offset].dist;
			 	(*KNN)[i][j].index = distMatrix[i][j+offset].index;
			}	
		}
	/* Step 4:  Free distance matrix, AFTER we are done with the sorting and stuff */
	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);
}  

/*
	Performs the knn calcuation as well as the final KNN array-merging. 
*/
void update_knn(DataSet *localDataSet, DataSet *inputDataSet, int K, /*in-out*/ nnPoint*** KNN){
	
	nnPoint** newKNN;
	knn(localDataSet, inputDataSet, K, &newKNN);

	// Merge Knn's
	int i;
	// TODO: Make this in parallel
	for(i=0; i<localDataSet->N; i++)
	{
		nnPoint *KNNMerged;
		// KNN[i] is **nnPoint type and newKNN[i] is 
		mergesort_nnpoint_arrays( &((*KNN)[i]), &(newKNN[i]), K, &KNNMerged); 

		// Throw out old KNN list of the i-th datapoint.
		free( (*KNN)[i]);
		free( newKNN[i]);

		// Replace old pointer with the merged Pointer.
		(*KNN)[i] = KNNMerged;
		
	}

	free(newKNN);
}


/**
	distance_matrix calculates all distances between all datapoints and stores 
	them in matric distMatrix [N,N] in an efficient way.

	distMatrix is symmetric with its diagonal==0. Thus, the calculation is 
	done efficiently, by performing N(N-1)/2 calc_dist calls instead of N^2.  
*/
void distance_matrix_SEQ(DataSet *localDataSet, DataSet *givenDataSet,  nnPoint*** distMatrix)
{		
	int N = localDataSet->N;
	int N2 = givenDataSet->N;
	int D = localDataSet->D;
	
	*distMatrix = (nnPoint**) malloc(N*sizeof(nnPoint*));		
	int i;
	for(i=0; i<N; i++)
		(*distMatrix)[i] = (nnPoint*) malloc(N2*sizeof(nnPoint));

	for(i=0; i<N; i++)
	{
		int j;
		for(j=0; j<N2; j++)
		{
			double d = calc_dist( localDataSet->dataPoints[i], givenDataSet->dataPoints[j], D);
			(*distMatrix)[i][j].dist = d;
			(*distMatrix)[i][j].index =  givenDataSet->index[j];
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
void distance_matrix_OMP(DataSet *localDataSet, DataSet *givenDataSet,  nnPoint*** distMatrix)
{
	omp_set_num_threads(4);

	int N = localDataSet->N;
	int N2 = givenDataSet->N;
	int D = localDataSet->D;
	
	// Dist Matrix: [N1 x N2] 
	*distMatrix = (nnPoint**) malloc(N*sizeof(nnPoint*));	
	
	int i;
	for(i=0; i<N; i++)
		(*distMatrix)[i] = (nnPoint*) malloc(N2*sizeof(nnPoint));
	
	/* This for-loop pairs the nested loops into workload of equal size.
		 	1st j-loop with Nth loop, 2nd with (N-1)th etc.
		 	That way eac thread performs N calculation in total.  */
	#pragma omp parallel for schedule(auto) num_threads(4)
		for(i=0; i<N; i++)
		{
			int j;
			for(j=0; j<N2; j++)
			{
				double d = calc_dist( localDataSet->dataPoints[i], givenDataSet->dataPoints[j], D);
				(*distMatrix)[i][j].dist = d;
				(*distMatrix)[i][j].index =  givenDataSet->index[j];
			}
		}
}


/**
	Calculates distance between datapoint A and B. 
	Each of these are double arrays of length D.
*/
double calc_dist(double* A, double* B, int D){
	
	int i;
	double temp, dist = 0;
	for(i=0; i<D; i++){
		temp = A[i]-B[i]; 
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
