CFLAGS   = -fPIC -O2 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS  = -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)

######## VARS

export DIR
export CFLAGS
export LDFLAGS

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/papi/generics.o \
               $(SRCDIR)/metrics/papi/cache.o \
               $(SRCDIR)/metrics/papi/flops.o \
               $(SRCDIR)/metrics/papi/instructions.o \
               $(SRCDIR)/metrics/papi/stalls.o \
               $(SRCDIR)/control/frequency.o

COMMON_OBJS = $(SRCDIR)/common/shared_configuration.o \
              $(SRCDIR)/common/ear_daemon_client.o \
              $(SRCDIR)/common/environment.o

TYPES = $(SRCDIR)/common/types.a

TRACE_OBJS = states/states.o \
             tracer/tracer.o \
             mpi_intercept/ear_api.o 

OBJS = dynais/dynais.o               \
       tracer/tracer.o               \
       states/states.o               \
       metrics/metrics.o             \
       models/models.o               \
       models/min_time.o             \
       models/min_energy.o           \
       models/monitoring.o           \
       models/sig_projections.o	     \
       mpi_intercept/process_MPI.o   \
       mpi_intercept/MPI_intercept.o \
       mpi_intercept/MPI_interface.o \
       mpi_intercept/ear_api.o

######## RULES

all: libear.so libeart.so 

%.a: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

update-trace-objs:
	@ $(MAKE) -B $(TRACE_OBJS)

update-trace-objs-gui:
	@ $(MAKE) -B $(TRACE_OBJS) CFLAGS='$(CFLAGS) -DEAR_GUI'

libear.so: update-trace-objs $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS) $(TYPES)
	$(CC) $(CFLAGS) -o libear.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS) $(TYPES)

libeart.so: update-trace-objs-gui
	$(CC) $(CFLAGS) -DEAR_GUI -o libeart.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS) $(TYPES)

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so
	mv libeart.so $(DESTDIR)/lib/libeart.so

clean:
	rm -f $(BINARIES) $(OBJS)
