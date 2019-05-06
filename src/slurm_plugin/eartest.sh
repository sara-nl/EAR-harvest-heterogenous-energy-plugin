#!/bin/bash

export TEST_APP='sleep 0'
export SLURM_COMP_TEST=1 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=2 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=3 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=4 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=5 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=6 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=7 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=8 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=9 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=10 && srun -n 1 $TEST_APP
export SLURM_COMP_TEST=11 && srun -n 1 $TEST_APP
