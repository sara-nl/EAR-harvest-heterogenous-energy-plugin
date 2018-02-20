DIR         = $(ROOTDIR)/tests/functionals
CFLAGS      = -I$(SRCDIR) -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon

######## FILES

NODE_ENERGY_LIB = $(ROOTDIR)/src/daemon/ear_node_energy_metrics.a

NODE_ENERGY_FLAGS = $(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(IPMI_LDFLAGS)

BINARIES = bandwith_monitor \
           eard_monitor \
           freeipmi_overhead \
           simd_power_metrics_openmp \
           energy_1_second \
           energy_updates_freq \
	   test_architecture

######## RULES

all: $(BINARIES)

bandwith_monitor: bandwith_monitor.c $(ROOTDIR)/src/daemon/ear_uncores.a $(ROOTDIR)/src/common/hardware.o
	$(CC) $(CFLAGS) -o bandwith_monitor bandwith_monitor.c \
	$(ROOTDIR)/src/daemon/ear_uncores.a $(ROOTDIR)/src/common/hardware.o

eard_monitor: eard_monitor.c $(ROOTDIR)/src/common/ear_daemon_client.o
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c \
	$(ROOTDIR)/src/common/ear_daemon_client.o

freeipmi_overhead: freeipmi_overhead.c $(NODE_ENERGY_LIB)  $(ROOTDIR)/src/common/hardware.o
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) $(IPMI_CFLAGS) -o freeipmi_overhead freeipmi_overhead.c \
	$(PAPI_LDFLAGS) $(NODE_ENERGY_FLAGS) $(ROOTDIR)/src/common/hardware.o

simd_tests.o: simd_tests.c
	gcc $(CFLAGS) -O0 -malign-double -march=native -c simd_tests.c

simd_power_metrics_openmp: simd_power_metrics_openmp.c simd_tests.o $(NODE_ENERGY_LIB)
	icc -qopenmp -axCORE_AVX512 $(CFLAGS) $(PAPI_CFLAGS) -I$(ROOTDIR)/src/library \
	-o simd_power_metrics_openmp simd_power_metrics_openmp.c \
	$(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) simd_tests.o \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o \
	$(ROOTDIR)/src/daemon/ear_frequency.o \
	$(ROOTDIR)/src/daemon/ear_turbo.o \
	$(ROOTDIR)/src/common/hardware.o \
	$(ROOTDIR)/src/metrics/instructions.o \
	$(ROOTDIR)/src/metrics/stalls.o \
	$(ROOTDIR)/src/metrics/flops.o \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(IPMI_LDFLAGS)

energy_1_second:energy_1_second.c $(NODE_ENERGY_LIB) 
	gcc -o energy_1_second $(CFLAGS)  energy_1_second.c $(ROOTDIR)/src/common/hardware.o \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(IPMI_LDFLAGS)

energy_updates_freq:energy_updates_freq.c $(NODE_ENERGY_LIB) 
	gcc -o energy_updates_freq $(CFLAGS)  energy_updates_freq.c $(ROOTDIR)/src/common/hardware.o \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(IPMI_LDFLAGS)

test_architecture:test_architecture.c $(ROOTDIR)/src/common/hardware.o
	gcc $(PAPI_CFLAGS) $(CFLAGS) -o test_architecture test_architecture.c \
	$(ROOTDIR)/src/common/hardware.o $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(IPMI_LDFLAGS)

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
