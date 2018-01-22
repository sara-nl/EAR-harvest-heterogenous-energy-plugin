DIR     = $(ROOTDIR)/src/library
LDFLAGS = -O2 -shared $(PAPI_LDFLAGS) $(CPUF_LDFLAGS)
CFLAGS  = -I$(DIR) $(PAPI_CFLAGS) $(CPUF_CFLAGS) -I$(ROOTDIR)/src/common -fPIC $(CONSTANTS) $(DEBUG)

######## VARS

export DIR
export CFLAGS
export LDFLAGS

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/ear_daemon_client.o \
             $(ROOTDIR)/src/common/environment.o \
			 $(ROOTDIR)/src/common/hardware.o

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
       ear_metrics/ear_papi.o            \
       ear_metrics/ear_node_energy.o     \
       ear_metrics/ear_turbo_metrics.o   \
       ear_metrics/ear_flops_metrics.o  \
       ear_metrics/ear_cache.o 

BINARIES = libear.so

######## RULES

.PHONY:

all: libear.so

%.a: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

libear.so: $(OBJS) $(OUTER_OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o libear.so $(OBJS) $(OUTER_OBJS) $(LDFLAGS)
	

libeart.so: $(OBJS) $(OUTER_OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o libeart.so $(OBJS) $(OUTER_OBJS) $(LIBRARIES) $(LDFLAGS)

######## OPTIONS

install:
	mv libear.so $(DESTDIR)/lib/libear.so

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
