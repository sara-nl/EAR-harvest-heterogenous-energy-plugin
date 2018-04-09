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

COMMON_OBJS = $(SRCDIR)/common/ear_daemon_client.o \
              $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/types.a

TRACE_OBJS = states/states.o \
             tracer/tracer.o \
             mpi_intercept/ear_api.o

TRACE_SRCS = states/states.c tracer/trace.c mpi_intercept/mpi_intercept.c


OBJS = dynais/dynais.o               \
       tracer/tracer.o               \
	   states/states.o               \
       models/models.o               \
       metrics/metrics.o             \
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

libear.so: $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)
	$(CC) $(CFLAGS) -o libear.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)

update-trace-objs:
	make -B $(TRACE_OBJS) CFLAGS='$(CFLAGS) -DEAR_GUI'

libeart.so: update-trace-objs $(TRACE_OBJS)
	$(CC) $(CFLAGS) -DEAR_GUI -o libeart.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so
	mv libeart.so $(DESTDIR)/lib/libeart.so

clean:
	rm -f $(BINARIES) $(OBJS)
