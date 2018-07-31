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
           $(SRCDIR)/daemon/eard_api.o \
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
libs_ALL += libear.so libeart.so
impi_ALL = all.impi.components libear.so \
           allt.impi.components libeart.so
endif

ifdef OMPICC
libs_ALL += libear_ompi.so libeart_ompi.so
ompi_ALL = all.ompi.components libear_ompi.so \
           allt.ompi.components libeart_ompi.so
endif
OMPI_CCFLAGS
######## RULES

all: all.gnrc.components $(impi_ALL) $(ompi_ALL)

%.so: $(OBJS) $(lib_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $(LDFLAGS) $(OBJS) $(lib_OBJS)

######## OPTIONS

install:
	cp $(libs_ALL) $(DESTDIR)/lib/

clean: clean.gnrc.components clean.impi.components
	rm -f *.o *.a *.so

depend: depend.gnrc.components depend.impi.components

depend-clean: depend-clean.gnrc.components depend-clean.impi.components

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
