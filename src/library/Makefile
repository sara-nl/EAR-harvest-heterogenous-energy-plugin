CFLAGS  = -fPIC -O3 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS) -I /hpc/base/intel/compilers_and_libraries_2019.4.243/linux/mpi/intel64/include
CFLAGS  = -fPIC -O3 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS) -I /hpc/base/ctt/packages/openmpi/openmpi-3.1.1_slurm/include
LDFLAGS = -shared $(PAPI_LDFLAGS) -ldl
MPICC = /hpc/base/intel/compilers_and_libraries_2019.4.243/linux/bin/intel64/icc
MPICC = /hpc/base/ctt/packages/openmpi/openmpi-3.1.1_slurm/bin/mpicc
MPICC = /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc

######## VARS

export DIR
export CFLAGS
export LDFLAGS

########

ifdef MPI_VERSION
ext = ${MPI_VERSION}.so
else
ext = so
endif

ifdef MPICC
libs_BINS += libear.$(ext)
libs_PATH  = $(DESTDIR)/lib
libs_OBJS  = all.components libear.so
libs_PERM  = 0755
endif

######## FILES

comm_OBJS = \
    $(SRCDIR)/daemon/eard_api.o \
    $(SRCDIR)/daemon/shared_configuration.o \
    $(SRCDIR)/metrics/libmetrics.a \
    $(SRCDIR)/common/libcommon.a

mpix_OBJS = \
    metrics/metrics.o \
    models/models.o \
    policies/policy.o \
    states/states.o \
    states/states_periodic.o \
    tracer/tracer.o \
    dynais/dynais.a \
    mpi_intercept/ear_api.o \
    api/api.o \
    api/earc_syms.o \
    api/earf_syms.o


ifdef NO
    mpi_intercept/ear_api.o \
    mpi_intercept/process_MPI.o \
mpic_OBJS = \
    mpi_intercept/mpi_definitions_c.o \
    mpi_intercept/mpi_interception_c.o \
    mpi_intercept/MPI_interface.o

ifeq ($(FEAT_FORT), 1)
mpif_OBJS = \
    mpi_intercept/mpi_definitions_f.o \
    mpi_intercept/mpi_interception_f.o \
    mpi_intercept/MPI_interfaceF.o
endif
endif

######## RULES

all: all.components $(libs_OBJS)

%.so: $(mpix_OBJS) $(comm_OBJS)
	$(MPICC) $(MPICC_FLAGS) $(CFLAGS) -o $*.$(ext) $(mpix_OBJS) $(mpic_OBJS) $(mpif_OBJS) \
	$(comm_OBJS) $(LDFLAGS)


######## OPTIONS

install: install.components libs.ginstall;

%.install: ;

clean: clean.components rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'dynais' $* || exit
	@ $(MAKE) -C 'metrics' $* || exit
	@ $(MAKE) -C 'models' $* || exit
	@ $(MAKE) -B -C 'policies' $* MPICC='$(MPICC)' || exit
	@ $(MAKE) -B -C 'states' $* MPICC='$(MPICC)' || exit
	@ $(MAKE) -B -C 'tracer' $* MPICC='$(MPICC)' || exit
#	@ $(MAKE) -B -C 'mpi_intercept' $* MPICC='$(MPICC)' || exit

######## DEPENDENCIES

depend: depend.components;

depend-clean: depend-clean.components;

include $(SRCDIR)/Makefile.extra
