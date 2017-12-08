
#include <stdlib.h>
#include <stdio.h>

//#include <boost/smart_ptr/shared_array.hpp>

#include "data_types.h"



int main(int argc, char** argv){
	
	int N,D;	
	dataPoint *dataSet;

	N = 10;
	D = 2;
	
	
	readData(&dataSet, N,D);

	int i;
	for (i = 0; i<N; i++){
		printDataPoint(dataSet[i],D);
		printf("\n");
	}

	return 0;
}


