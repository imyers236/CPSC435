#!/bin/bash
#
#SBATCH --job-name=mpi_hello
#SBATCH --output=mpi_hello.txt
#SBATCH --nodes=2
#SBATCH --tasks-per-node=4
#SBATCH --time=05:00
mpiexec mpi_hello