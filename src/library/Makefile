CFLAGS   = -fPIC -O2 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS  = -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)

######## VARS

export DIR
export CFLAGS
export LDFLAGS

######## FILES

lib_OBJS = $(SRCDIR)/metrics/custom/hardware_info.o \
           $(SRCDIR)/metrics/papi/generics.o \
           $(SRCDIR)/metrics/papi/cache.o \
           $(SRCDIR)/metrics/papi/flops.o \
           $(SRCDIR)/metrics/papi/instructions.o \
           $(SRCDIR)/metrics/papi/stalls.o \
           $(SRCDIR)/control/frequency.o \
           $(SRCDIR)/daemon/eard_client_api.o \
           $(SRCDIR)/daemon/shared_configuration.o \
           $(SRCDIR)/common/environment_common.o \
           $(SRCDIR)/common/file_lock.o \
           $(SRCDIR)/common/environment.o \
           $(SRCDIR)/common/math_operations.o \
           $(SRCDIR)/common/types.a

OBJS = dynais/dynais.o \
       metrics/metrics.o \
       models/models.o \
       models/min_time.o \
       models/min_energy.o \
       models/monitoring.o \
       models/sig_projections.o \
       states/states.o \
       states/states_periodic.o \
       tracer/tracer.o \
       mpi_intercept/ear_api.o \
       mpi_intercept/process_MPI.o \
       mpi_intercept/MPI_intercept.o \
       mpi_intercept/MPI_interface.o

ifdef MPICC
impi_ALL = all.impi.components libear.so \
           allt.impi.components libeart.so
endif

ifdef OMPICC
ompi_ALL = all.ompi.components libear_ompi.so \
           allt.ompi.components libeart_ompi.so
endif

######## RULES

all: all.gnrc.components $(impi_ALL) $(ompi_ALL)

%.so:
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $(LDFLAGS) $(OBJS) $(lib_OBJS)

######## OPTIONS

install:

clean: clean.gnrc.components clean.impi.components
	rm -f *.o *.a *.so

depend: depend.gnrc.components depend.impi.components

######## SUBFOLDERS

%.gnrc.components:
	$(MAKE) -C 'dynais' $*
	$(MAKE) -C 'metrics' $*
	$(MAKE) -C 'models' $*

%.impi.components:
	$(MAKE) -B -C 'mpi_intercept' $* MPICC='$(MPICC)'
	$(MAKE) -B -C 'states' $* MPICC='$(MPICC)'
	$(MAKE) -B -C 'tracer' $* MPICC='$(MPICC)'

%.ompi.components:
	$(MAKE) -B -C 'mpi_intercept' $* MPICC='$(OMPICC)'
	$(MAKE) -B -C 'states' $* MPICC='$(OMPICC)'
	$(MAKE) -B -C 'tracer' $* MPICC='$(OMPICC)'
