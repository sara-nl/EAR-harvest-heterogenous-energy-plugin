#!/bin/bash
autoreconf -i
./configure --with-papi=/hpc/base/ctt/packages/papi/5.4.3 --with-gsl=/hpc/base/ctt/packages/gsl/1.16 --with-slurm=/home/xjaneas/slurm --with-freeipmi=/home/xjcorbalan --prefix=/home/xjcorbalan/my_ear
make
make install

