#!/bin/bash

mpi_proc=(1 2 4 8)
omp_num=(1 2 4 )

# Run Tests forvarious bitonic parallel implemantation

p=4
q=1
			
sed -i '5s/.*/\#PBS \-l nodes='$p'\:ppn='$q'/' nns_mpi_sampazio.sh
qsub -F "$p $q" nns_mpi_sampazio.sh

