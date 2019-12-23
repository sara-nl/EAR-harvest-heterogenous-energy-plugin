#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks=40
#SBATCH --tasks-per-node=20
#SBATCH --nodelist=cmp2771,cmp2772
#SBATCH --account=COLBSC
#SBATCH --time=5

#SLURM_COMP_VERBOSE=2 srun  ./erun --program=hostname --ear-verbose=1
#SLURM_COMP_VERBOSE=2 srun  ./erun --program=hostname --ear-verbose=1
SLURM_COMP_VERBOSE=2 srun --mpi=pmi2 ./erun --program=/hpc/base/ctt/packages/EAR/1908/bin/kernels/bt-mz.C.40 --ear-verbose=1
SLURM_COMP_VERBOSE=2 srun --mpi=pmi2 ./erun --program=/hpc/base/ctt/packages/EAR/1908/bin/kernels/sp-mz.C.40 --ear-verbose=1
