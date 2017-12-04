/**
 knn.c

 The knn library that handles the parallel
  or non parallel executio of the knn algorithm on a block of data

	Author: Savvas Sampaziotis
*/

#include "omp.h"
#include "stdio.h"

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
void knn(double** A, int N, int D, int K, int** KNN, double** KNN_dist){


	#pragma omp parallel
	{
		printf( "Savvas %d", omp_get_thread_num() );
	}



}  


/**
	Calculates distance between datapoint A and B. 
	Each of these are double arrays of length D.
*/
double calc_dist(double* A, double* B, int D){
	double dist;
	int i;
	dist = 0;

	for(i=0; i<D; i++){
		dist += (A[i]-B[i])*(A[i]-B[i]);
	}
	printf("Dist = %f\n", dist);
	return 0;
}




void test(int* a, int N) {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }
  printf(" TEST %s\n", (pass) ? "PASSed" : "FAILed");
}