DIR         = $(ROOTDIR)/tests/functionals
CFLAGS      = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon -march=native

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/hardware.o        \
             $(ROOTDIR)/src/common/environment.o     \
             $(ROOTDIR)/src/common/string_enhanced.o \
             $(ROOTDIR)/src/common/ear_daemon_client.o

BINARIES = intel_pci_uncores_stress eard_monitor papi_cpu_gflops simd_power_metrics freeipmi_overhead

######## RULES

all: $(BINARIES)

intel_pci_uncores_stress: intel_pci_uncores_stress.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o intel_pci_uncores_stress intel_pci_uncores_stress.c \
	$(OUTER_OBJS) $(ROOTDIR)/src/daemon/ear_uncores.a

eard_monitor: eard_monitor.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c $(OUTER_OBJS)

papi_cpu_gflops: papi_cpu_gflops.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_cpu_gflops papi_cpu_gflops.c \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) -lpthread

simd_power_metrics: simd_power_metrics.c $(OUTER_OBJS) $(ENERGY_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o simd_power_metrics simd_power_metrics.c \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o $(ROOTDIR)/src/daemon/ear_node_energy_metrics.a \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) -lpthread

freeipmi_overhead: freeipmi_overhead.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) $(IPMI_CFLAGS) -o freeipmi_overhead freeipmi_overhead.c \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(OUTER_OBJS) $(PAPI_LDFLAGS) $(IPMI_LDFLAGS)

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
