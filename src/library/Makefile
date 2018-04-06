CFLAGS  = -fPIC -O2 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS = -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)
DEBUG   = -DEAR_GUI

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

COMMON_OBJS = $(SRCDIR)/common/ear_daemon_client.o \
              $(SRCDIR)/common/environment.o \
			  $(SRCDIR)/common/shared_configuration.o

TYPES = $(SRCDIR)/common/types.a

TRACE_ON_OBJS = states/states_trace.o \
             tracer/tracer_trace.o \
             mpi_intercept/ear_api_trace.o 

TRACE_OFF_OBJS = states/states.o \
             tracer/tracer.o \
             mpi_intercept/ear_api.o 

TRACE_SRCS = states/states.c \
			tracer/tracer.c \
			mpi_intercept/ear_api.c

OBJS = dynais/dynais.o               \
       models/models.o               \
       models/monitoring.o               \
       models/min_energy.o               \
       models/min_time.o               \
	   models/sig_projections.o		\
       metrics/metrics.o             \
       mpi_intercept/process_MPI.o   \
       mpi_intercept/MPI_intercept.o \
       mpi_intercept/MPI_interface.o 

BINARIES = libear.so libeart.so

######## RULES

.PHONY:

all: $(BINARIES) 

%.a: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%_trace.o:%.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

libear.so: $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS) $(TRACE_OFF_OBJS) $(TYPES)
	$(CC) $(CFLAGS) -o libear.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS) $(TRACE_OFF_OBJS) $(TYPES)

update-trace-objs:
	touch $(TRACE_SRCS)
	export DEBUG

libeart.so: update-trace-objs $(TRACE_ON_OBJS) $(TYPES)
	$(CC) $(CFLAGS) -o libeart.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS) $(TRACE_ON_OBJS) $(TYPES)
	touch $(TRACE_SRCS)

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so
	mv libeart.so $(DESTDIR)/lib/libeart.so


clean:
	rm -f $(BINARIES) $(OBJS) $(TRACE_OFF_OBJS) $(TRACE_ON_OBJS)
