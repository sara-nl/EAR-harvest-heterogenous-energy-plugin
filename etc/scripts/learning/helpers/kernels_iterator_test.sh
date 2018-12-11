#!/bin/bash

# Non edit region
source $EAR_INSTALL_PATH/bin/scripts/learning/helpers/kernels_test.sh

# Running the learning phase
for (( j=0; j<$EAR_TIMES; j++ ))
do
        test_phase lu-mpi C
        test_phase ep D
        test_phase bt-mz C
        test_phase sp-mz C
        test_phase lu-mz C
        test_phase ua C
        test_phase dgemm
        test_phase stream
done

