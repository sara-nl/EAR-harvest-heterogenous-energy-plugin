DIR    = $(ROOTDIR)/tests/functionals
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/daemon/ear_uncores.a       \
             $(ROOTDIR)/src/daemon/ear_node_energy_metrics.a \
             $(ROOTDIR)/src/common/ear_arch_type.o     \
             $(ROOTDIR)/src/common/ear_daemon_client.o \
             $(ROOTDIR)/src/common/ear_configuration.o

BINARIES = 

######## RULES

all: $(BINARIES)

intel_pci_uncores_stress: intel_pci_uncores_stress.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o intel_pci_uncores_stress intel_pci_uncores_stress.c $(OUTER_OBJS)

daemon_monitoring: daemon_monitoring.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o daemon_monitoring daemon_monitoring.c $(OUTER_OBJS)

papi_cpu_gflops: papi_cpu_gflops.c
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_cpu_gflops papi_cpu_gflops.c $(PAPI_LDFLAGS) -lpthread

instructions_energy: instructions_energy.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o instructions_energy instructions_energy.c \
	$(PAPI_LDFLAGS) $(OUTER_OBJS) -lpthread -msse2 -mavx -mfma

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
