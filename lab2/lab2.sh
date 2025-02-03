#!/bin/bash
#
#SBATCH --job-name=lab2
#SBATCH --output=lab2.txt
#SBATCH --nodes=2
#SBATCH --tasks-per-node=4
#SBATCH --time=05:00
mpiexec lab2