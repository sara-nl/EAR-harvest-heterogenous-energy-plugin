CFLAGS   = -fPIC -O2 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS  = -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)

######## FILES

HEADERS = $(SRCDIR)/library/common/externs.h \
          $(SRCDIR)/common/ear_verbose.h \
          $(SRCDIR)/common/states.h \
          $(SRCDIR)/common/states_periodic.h \
          $(SRCDIR)/common/config.h

METRICS_OBJS = $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/papi/generics.o \
               $(SRCDIR)/metrics/papi/cache.o \
               $(SRCDIR)/metrics/papi/flops.o \
               $(SRCDIR)/metrics/papi/instructions.o \
               $(SRCDIR)/metrics/papi/stalls.o \
               $(SRCDIR)/control/frequency.o

COMMON_OBJS = $(SRCDIR)/daemon/eard_api.o \
              $(SRCDIR)/common/environment_common.o \
              $(SRCDIR)/common/file_lock.o \
              $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/math_operations.o \
              $(SRCDIR)/common/types.a

ifdef FEAT_SHRMEM
COMMON_OBJS += $(SRCDIR)/daemon/shared_configuration.o
endif

OUTER_OBJS = $(COMMON_OBJS) $(METRICS_OBJS)

REMAKE_OBJS = states/states.o \
			  states/states_periodic.o \
              tracer/tracer.o \
              mpi_intercept/ear_api.o \
              mpi_intercept/process_MPI.o \
              mpi_intercept/MPI_intercept.o \
              mpi_intercept/MPI_interface.o

OBJS = dynais/dynais.o \
       metrics/metrics.o \
       models/models.o \
       models/min_time.o \
       models/min_energy.o \
       models/monitoring.o \
       models/sig_projections.o 

######## VARS

export DIR
export CFLAGS
export LDFLAGS

######## RULES

all: $(OBJS) libear.so libeart.so  

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

######## MPI RULES

ifdef MPICC

update-mpi-objs:
	@ $(MAKE) -B $(REMAKE_OBJS) MPICC='$(MPICC)'

update-mpi-objs-trace:
	@ $(MAKE) -B $(REMAKE_OBJS) MPICC='$(MPICC)' CFLAGS='$(CFLAGS) -DEAR_GUI'

libear.so: update-mpi-objs 
	$(CC) $(CC_FLAGS) $(CFLAGS) -o libear.so $(LDFLAGS) $(REMAKE_OBJS) $(OBJS) $(OUTER_OBJS)

libeart.so: update-mpi-objs-trace
	$(CC) $(CC_FLAGS) $(CFLAGS) -o libeart.so $(LDFLAGS) $(REMAKE_OBJS) $(OBJS) $(OUTER_OBJS)

endif

######## SPECIFIC OPEN MPI RULES

ifdef OMPICC

update-ompi-objs:
	@ $(MAKE) -B $(REMAKE_OBJS) MPICC='$(OMPICC)'

update-ompi-objs-trace:
	@ $(MAKE) -B $(REMAKE_OBJS) MPICC='$(OMPICC)' CFLAGS='$(CFLAGS) -DEAR_GUI'

: update-ompi-objs
	$(CC) $(CC_FLAGS) $(CFLAGS) -o  $(LDFLAGS) $(REMAKE_OBJS) $(OBJS) $(OUTER_OBJS)

: update-ompi-objs-trace
	$(CC) $(CC_FLAGS) $(CFLAGS) -o  $(LDFLAGS) $(REMAKE_OBJS) $(OBJS) $(OUTER_OBJS)

endif

######## OPTIONS

install:
ifdef MPICC
	@ mv libear.so $(DESTDIR)/lib/libear.so
	@ mv libeart.so $(DESTDIR)/lib/libeart.so
endif
ifdef OMPICC
	@ mv  $(DESTDIR)/lib/
	@ mv  $(DESTDIR)/lib/
endif

clean:
	rm -f *.so *.a *.o
	rm $(REMAKE_OBJS) $(OBJS)
