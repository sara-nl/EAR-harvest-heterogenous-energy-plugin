#!/bin/bash



# Edit minimum frequency (in KHz)
export MIN_FREQUENCY=1700000

# Edit output options
export OUT_COE="$HOME/EAR/etc/ear/coeffs"
mkdir $OUT_COE

# Non-edit region

echo "Computing coefficients for node=$SLURMD_NODENAME"
srun -N 1 -n 1  -w $SLURMD_NODENAME  \
        $EAR_INSTALL_PATH/bin/tools/coeffs_compute $OUT_COE $MIN_FREQUENCY $SLURMD_NODENAME

