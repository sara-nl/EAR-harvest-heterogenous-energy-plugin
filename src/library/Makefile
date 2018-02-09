DIR     = $(ROOTDIR)/src/library
LDFLAGS = -O2 -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)
CFLAGS  = -fPIC $(CONSTANTS) $(DEBUG) -I$(DIR)
CFLAGS += -I$(DIR)/common -I$(ROOTDIR)/src/common
CFLAGS += $(PAPI_CFLAGS) $(CPUF_CFLAGS)

######## VARS

export DIR
export CFLAGS
export LDFLAGS

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/ear_daemon_client.o \
             $(ROOTDIR)/src/common/environment.o \
             $(ROOTDIR)/src/common/hardware.o \
             $(ROOTDIR)/src/common/types.a

OBJS = ear_states/ear_states.o           \
       ear_db/ear_db.o                   \
       ear_dynais/ear_dynais.o           \
       ear_dyn_inst/process_MPI.o        \
       ear_dyn_inst/ear_api.o            \
       ear_dyn_inst/MPI_intercept.o      \
       ear_dyn_inst/MPI_interface.o      \
       ear_frequency/ear_cpufreq.o       \
       ear_gui/ear_gui.o                 \
       ear_models/ear_models.o           \
       ear_metrics/ear_metrics.o            \
       ear_metrics/ear_node_energy.o     \
       ear_metrics/ear_turbo_metrics.o   \
       ear_metrics/ear_flops_metrics.o   \
       ear_metrics/ear_cache.o           \
       ear_metrics/ear_basic.o           \
       ear_metrics/ear_stalls.o

BINARIES = libear.so libeart.so

######## RULES

.PHONY:

all: libear.so 
traces: libeart.so

%.a: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

libear.so: $(OBJS) $(OUTER_OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o libear.so $(OBJS) $(OUTER_OBJS) $(LDFLAGS)

libeart.so: $(OBJS) $(OUTER_OBJS) $(LIBRARIES)
	touch ear_states/ear_states.c
	touch ear_dyn_inst/ear_api.c
	touch ear_gui/ear_gui.c
	$(MAKE) -C ear_states CFLAGS='$(CFLAGS) -DEAR_GUI' ear_states.o
	$(MAKE) -C ear_gui CFLAGS='$(CFLAGS) -DEAR_GUI' ear_gui.o
	$(MAKE) -C ear_dyn_inst CFLAGS='$(CFLAGS) -DEAR_GUI' ear_api.o
	$(CC) $(CFLAGS) -o libeart.so $(OBJS) $(OUTER_OBJS) $(LIBRARIES) $(LDFLAGS)
	touch ear_states/ear_states.c
	touch ear_dyn_inst/ear_api.c
	touch ear_gui/ear_gui.c

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so

install_traces:
	mv libeart.so $(DESTDIR)/lib/libeart.so
	

SUB_DIRS = ear_states    \
           ear_db        \
           ear_dyn_inst  \
           ear_dynais    \
           ear_frequency \
           ear_gui       \
           ear_metrics   \
           ear_models

clean:
	@ for i in $(SUB_DIRS); do    \
		$(MAKE) -C $${i} clean; \
	done
	rm -f $(BINARIES)
