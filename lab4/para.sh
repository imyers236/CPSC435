#!/bin/bash
#
#SBATCH --job-name=sort
#SBATCH --output=para.txt
#SBATCH --nodes=4
#SBATCH --tasks-per-node=1
#SBATCH --time=05:00
mpiexec para 48