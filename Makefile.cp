CC           = icc
CC           = gcc
CC_FLAGS     = -static-intel
MPICC        = /hpc/opt/intel/compilers_and_libraries_2017.7.259/linux/mpi/intel64/bin/mpiicc
MPICC_FLAGS  = -static-intel -Wall -Werror-all
MPI_VERSION  = 
ROOTDIR      = $(shell pwd)
SRCDIR       = $(ROOTDIR)/src
DESTDIR      = /hpc/opt/ear
ETCDIR       = /hpc/opt/ear/etc
TMPDIR       = /home/xjaneas
DOCDIR       = /share/doc/ear
PAPI_BASE    = /hpc/opt/papi
PAPI_CFLAGS  = -I/hpc/opt/papi/include
PAPI_LDFLAGS = -L/hpc/opt/papi/lib -Wl,-rpath,/hpc/opt/papi/lib -lpapi
GSL_BASE     = /hpc/opt/gsl
GSL_CFLAGS   = -I/hpc/opt/gsl/include
GSL_LDFLAGS  = -L/hpc/opt/gsl/lib -lm -lgsl -lgslcblas
SLURM_BASE   = /hpc/opt/slurm
SLURM_CFLAGS = -I/hpc/opt/slurm/include
DB_BASE      = /usr
DB_CFLAGS    = 
DB_LDFLAGS   = -L/usr/lib64/mysql -lmysqlclient
CHOWN_USR    = xjaneas
CHOWN_GRP    = 
CONSTANTS    = -DSEC_KEY=10001
REPLACE      =
FEAT_AVX512  = -DAVX512

######## VARS

export CC
export CC_FLAGS
export MPICC
export MPICC_FLAGS
export MPI_VERSION
export ROOTDIR
export SRCDIR
export DESTDIR
export ETCDIR
export TMPDIR
export DOCDIR
export PAPI_BASE
export PAPI_CFLAGS
export PAPI_LDFLAGS
export GSL_BASE
export GSL_CFLAGS
export GSL_LDFLAGS
export SLURM_BASE
export SLURM_CFLAGS
export DB_BASE
export DB_CFLAGS
export DB_LDFLAGS
export CHOWN_USR
export CHOWN_GRP
export CONSTANTS
export REPLACE
export FEAT_AVX512

######## RULES

.PHONY: check functionals

all:
	@ $(MAKE) -C 'src' all || exit

full: clean all 

check:
	@ $(MAKE) -C 'src' check

install:
	@ $(MAKE) -C 'etc' install || exit
	@ $(MAKE) -C 'src' install || exit

clean:
	@ $(MAKE) -C 'src' clean || exit

########

devel.install: install;
	@ $(MAKE) -C 'etc' devel.install || exit
	@ $(MAKE) -C 'src' devel.install || exit

etc.install: ;
	@ $(MAKE) -C 'etc' etc.install || exit
	@ $(MAKE) -C 'src' etc.install || exit

rpm.install: install;
	@ $(MAKE) -C 'etc' rpm.install || exit

scripts.install: ;
	@ $(MAKE) -C 'etc' scripts.install || exit

doc.install: ;
	@ $(MAKE) -C 'doc' install || exit 

%.install: ;
	@ $(MAKE) -C src $*.install || exit

########

depend:
	@ $(MAKE) -C 'src' depend || exit

depend-clean:
	@ $(MAKE) -C 'src' depend-clean || exit
