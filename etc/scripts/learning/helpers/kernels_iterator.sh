#!/bin/bash

# Non edit region
source $EAR_INSTALL_PATH/bin/scripts/learning/helpers/kernels_executor.sh

# Running the learning phase
for (( i=$EAR_MIN_P_STATE; i<=$EAR_MAX_P_STATE; i++ ))
do
    for (( j=0; j<$EAR_TIMES; j++ ))
    do
        export EAR_P_STATE=$i
        learning_phase lu-mpi C
        learning_phase ep D
        learning_phase bt-mz C
        learning_phase sp-mz C
        learning_phase lu-mz C
        learning_phase ua C
        learning_phase dgemm
        learning_phase stream
    done
done

if [ $EAR_LEARNING_PHASE -eq 1 ]
then
	# Calculating coefficients
	$EAR_INSTALL_PATH/bin/compute_coefficients "$EAR_DB_PATHNAME`hostname -a`.db.csv" \
     	$EAR_COEFF_DB_PATHNAME`hostname -a` 1000000 `hostname -a`
fi
