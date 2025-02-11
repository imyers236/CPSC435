#!/bin/bash
#
#SBATCH --job-name=matMul
#SBATCH --output=matrixMul.txt
#SBATCH --nodes=2
#SBATCH --tasks-per-node=4
#SBATCH --time=05:00
mpiexec matMul 512