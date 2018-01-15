#!/bin/bash
#PBS -q pdlab
#PBS -N sampazio_mpiknn
#PBS -j oe
#PBS -l nodes=3:ppn=8
#PBS -l walltime=0:25:00

module load mpi

cd $PBS_O_WORKDIR
export NP=$(cat $PBS_NODEFILE | wc -l)

export I2G_MPI_TYPE=mpich2
$I2G_MPI_START


export I2G_MPI_APPLICATION_ARGS="3 "+$2
export I2G_MPI_APPLICATION=./out/mpi_knn

echo "$1 $2" 

$I2G_MPI_START

#mpiexec -n $1 ./out/mpi_knn 3 $2


