#!/bin/bash

SLURM_COMP_VERBOSE=2
SLURM_HACK_LIBRARY=/hpc/opt/shared/xjaneas/libear.so

$PWD/erun --program=$1 $2 $3 $4 $5 --plugstack prefix=/hpc/opt/ear sysconfdir=/hpc/opt/ear/etc localstatedir=/var/run/ear default=on

$PWD/erun --program=$1 $2 $3 $4 $5 --plugstack prefix=/hpc/opt/ear sysconfdir=/hpc/opt/ear/etc localstatedir=/var/run/ear default=on
