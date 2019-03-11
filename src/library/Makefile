CFLAGS  = -fPIC -O3 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS = -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)

######## VARS

export DIR
export CFLAGS
export LDFLAGS

########

ifdef MPICC
libs_BINS += libear.so
libs_PATH  = $(DESTDIR)/lib
impi_OBJS  = all.impi.components
libs_PERM  = 0640
endif

ifdef OMPICC
libs_BINS += libear_ompi.so
libs_PATH  = $(DESTDIR)/lib
ompi_OBJS  = all.ompi.components
libs_PERM  = 0640
endif

######## FILES

comm_OBJS = $(SRCDIR)/metrics/libmetrics.a \
    $(SRCDIR)/control/frequency.o \
    $(SRCDIR)/daemon/eard_api.o \
    $(SRCDIR)/daemon/shared_configuration.o \
    $(SRCDIR)/common/environment_common.o \
    $(SRCDIR)/common/file.o \
    $(SRCDIR)/common/environment.o \
    $(SRCDIR)/common/math_operations.o \
    $(SRCDIR)/common/types.a

xmpi_OBJS = dynais/dynais.o \
    metrics/metrics.o \
    models/models.o \
    models/min_time.o \
    models/min_energy.o \
    models/monitoring.o \
    states/states.o \
    states/states_periodic.o \
    tracer/tracer.o \
    tracer/tracer_mpi.o \
    mpi_intercept/ear_api.o \
    mpi_intercept/freq_synchro.o \
    mpi_intercept/process_MPI.o

MPIC_OBJS = mpi_intercept/MPI_intercept.o \
    mpi_intercept/MPI_interface.o
	
MPIF_OBJS = mpi_intercept/MPI_interceptF.o \
    mpi_intercept/MPI_interfaceF.o

######## RULES

all: all.gnrc.components $(impi_OBJS) $(ompi_OBJS) $(libs_BIN)

%.ompi.so: $(xmpi_OBJS) $(comm_OBJS)
	$(OMPICC) $(OMPICC_FLAGS) $(CFLAGS) -o $*_ompi.so $(xmpi_OBJS) $(MPIC_OBJS) \
	$(MPIF_OBJS) $(comm_OBJS) $(LDFLAGS)

%.so: $(xmpi_OBJS) $(comm_OBJS)
	$(MPICC) $(MPICC_FLAGS) $(CFLAGS) -o $*.so $(xmpi_OBJS) $(MPIC_OBJS) \
	$(comm_OBJS) $(LDFLAGS)

######## OPTIONS

install: libs.ginstall;

%.install: ;

clean: clean.gnrc.components clean.impi.components rclean;

######## SUBFOLDERS

%.gnrc.components:
	@ $(MAKE) -C 'dynais' $* || exit
	@ $(MAKE) -C 'metrics' $* || exit
	@ $(MAKE) -C 'models' $* || exit

%.impi.components:
	@ $(MAKE) -B -C 'mpi_intercept' $* MPICC='$(MPICC)' || exit
	@ $(MAKE) -B -C 'states' $* MPICC='$(MPICC)' || exit
	@ $(MAKE) -B -C 'tracer' $* MPICC='$(MPICC)' || exit

%.ompi.components:
	@ $(MAKE) -B -C 'mpi_intercept' $* MPICC='$(OMPICC)' || exit
	@ $(MAKE) -B -C 'states' $* MPICC='$(OMPICC)' || exit
	@ $(MAKE) -B -C 'tracer' $* MPICC='$(OMPICC)' || exit

######## DEPENDENCIES

depend: depend.gnrc.components depend.impi.components

depend-clean: depend-clean.gnrc.components depend-clean.impi.components

include $(SRCDIR)/Makefile.depend
