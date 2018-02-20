DIR     = $(ROOTDIR)/src/library
LDFLAGS = -O2 -shared $(CPUF_LDFLAGS)
CFLAGS  = -fPIC $(CONSTANTS) $(DEBUG) -I$(SRCDIR) -I$(DIR)
CFLAGS += -I$(DIR)/common -I$(ROOTDIR)/src/common $(CPUF_CFLAGS)

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
               $(SRCDIR)/metrics/papi/stalls.o

COMMON_OBJS = $(SRCDIR)/common/ear_daemon_client.o \
              $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/types.a

OBJS = ear_states/ear_states.o           \
       ear_dynais/ear_dynais.o           \
       ear_dyn_inst/process_MPI.o        \
       ear_dyn_inst/ear_api.o            \
       ear_dyn_inst/MPI_intercept.o      \
       ear_dyn_inst/MPI_interface.o      \
       ear_frequency/ear_cpufreq.o       \
       ear_gui/ear_gui.o                 \
       ear_models/ear_models.o           \
       ear_metrics/ear_metrics.o

BINARIES = libear.so libeart.so

######## RULES

.PHONY:

all: libear.so 

traces: libeart.so

%.a: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

libear.so: $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)
	$(CC) $(CFLAGS) -o libear.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)

libeart.so: $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)
	touch ear_states/ear_states.c
	touch ear_dyn_inst/ear_api.c
	touch ear_gui/ear_gui.c
	$(MAKE) -C ear_states CFLAGS='$(CFLAGS) -DEAR_GUI' ear_states.o
	$(MAKE) -C ear_gui CFLAGS='$(CFLAGS) -DEAR_GUI' ear_gui.o
	$(MAKE) -C ear_dyn_inst CFLAGS='$(CFLAGS) -DEAR_GUI' ear_api.o
	$(CC) $(CFLAGS) -o libeart.so $(LDFLAGS) $(OBJS) $(COMMON_OBJS) $(METRICS_OBJS)
	touch ear_states/ear_states.c
	touch ear_dyn_inst/ear_api.c
	touch ear_gui/ear_gui.c

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so

install_traces:
	mv libeart.so $(DESTDIR)/lib/libeart.so

clean:
	rm -f $(BINARIES) $(OBJS)
