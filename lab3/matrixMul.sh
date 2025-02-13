#!/bin/bash
#
#SBATCH --job-name=matMul
#SBATCH --output=matrixMul.txt
#SBATCH --nodes=4
#SBATCH --tasks-per-node=1
#SBATCH --time=05:00
mpiexec matMul 16