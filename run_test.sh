#!/bin/bash

mpi_proc=(3)
omp_num=(1 2 4 8)

# Run Tests forvarious bitonic parallel implemantations
for p in "${mpi_proc[@]}"; do
	for q in "${omp_num[@]}"; do
			
		sed -i '5s/.*/\#PBS \-l nodes='$p'\:ppn='$q'/' nns_mpi_sampazio.sh
		
		qsub -F "$p $q" nns_mpi_sampazio.sh
		#qsub -F "$p $q" nns_mpi_sampazio.sh

		#echo $p $q	
	done	
done

