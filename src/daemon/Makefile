DIR     = $(ROOTDIR)/src/daemon
CFLAGS  = -I$(DIR) -I$(ROOTDIR)/src/common $(CONSTANTS)
LDFLAGS = $(PAPI_LDFLAGS) $(IPMI_LDFLAGS) $(CPUF_LDFLAGS)

######## VARS

export DIR

######## FILES

COMPONENTS = uncore_architectures node_energy_metrics

OUTER_OBJS = $(ROOTDIR)/src/common/ear_configuration.o

OBJS = ear_arch_type.o    \
       ear_frequency.o    \
       ear_rapl_metrics.o \
       ear_turbo.o

ENERGY_OBJS = node_energy_metrics/ibm.o        \
              node_energy_metrics/lenovo_act.o \
              node_energy_metrics/lenovo_wct.o \
              ear_node_energy_metrics.o

UNCORE_OBJS = uncore_architectures/pci_uncores.o \
              ear_uncores.o

LIBRARIES = ear_uncores.a ear_node_energy_metrics.a
BINARIES  = eard

######## RULES

all: $(BINARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

ear_turbo.o: ear_turbo.c ear_turbo.h
	$(CC) $(CFLAGS) -c ear_turbo.c

ear_frequency.o: ear_frequency.c ear_frequency.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) $(PAPI_CFLAGS) -c ear_frequency.c

ear_rapl_metrics.o: ear_rapl_metrics.c ear_rapl_metrics.h
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -c ear_rapl_metrics.c

ear_arch_type.o: ear_arch_type.c ear_arch_type.h
	$(CC) $(CFLAGS) -c ear_arch_type.c

ear_node_energy_metrics.o: ear_node_energy_metrics.c ear_node_energy_metrics.h
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) $(IPMI_CFLAGS) -c ear_node_energy_metrics.c

ear_node_energy_metrics.a: $(ENERGY_OBJS)
	ar rvs ear_node_energy_metrics.a $(ENERGY_OBJS)

ear_uncores.o: ear_uncores.c ear_uncores.h
	$(CC) $(CFLAGS) -c ear_uncores.c

ear_uncores.a: $(UNCORE_OBJS)
	ar rvs ear_uncores.a $(UNCORE_OBJS)

eard: ear_daemon.c $(OBJS) $(OUTER_OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(OBJS) $(OUTER_OBJS) $(LIBRARIES) $(LDFLAGS)

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	@ for i in $(COMPONENTS); do  \
		$(MAKE) -C $${i} clean; \
	done
	rm -rf $(OBJS)
	rm -rf $(ENERGY_OBJS)
	rm -rf $(UNCORE_OBJS)
	rm -rf $(LIBRARIES)
	rm -rf $(BINARIES)
