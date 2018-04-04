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
              $(SRCDIR)/common/types.a \
			  $(SRCDIR)/common/shared_configuration.o

TRACE_OBJS = ear_states/ear_states.o \
             ear_dyn_inst/ear_api.o \
             tracer/tracer.o

OBJS = dynais/dynais.o                   \
       tracer/tracer.o                   \
	   ear_states/ear_states.o           \
       ear_dyn_inst/process_MPI.o        \
       ear_dyn_inst/ear_api.o            \
       ear_dyn_inst/MPI_intercept.o      \
       ear_dyn_inst/MPI_interface.o      \
       ear_models/ear_models.o           \
       ear_metrics/ear_metrics.o

BINARIES = libear.so libeart.so

######## RULES

.PHONY:

all: $(BINARIES) 

%.a: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

libear.so: $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)
	$(CC) $(CFLAGS) -o libear.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)

update-trace-objs:
	touch $(TRACE_OBJS)
	export DEBUG

libeart.so: update-trace-objs $(TRACE_OBJS)
	$(CC) $(CFLAGS) -o libeart.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so
	mv libeart.so $(DESTDIR)/lib/libeart.so


clean:
	rm -f $(BINARIES) $(OBJS)
