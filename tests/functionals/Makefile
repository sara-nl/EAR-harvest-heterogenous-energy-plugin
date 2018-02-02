DIR         = $(ROOTDIR)/tests/functionals
CFLAGS      = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon

######## FILES

BINARIES = bandwith_monitor       \
           eard_monitor           \
           freeipmi_overhead      \
           simd_power_metrics_openmp

######## RULES

all: $(BINARIES)

bandwith_monitor: bandwith_monitor.c
	$(CC) $(CFLAGS) -o bandwith_monitor bandwith_monitor.c \
	$(ROOTDIR)/src/daemon/ear_uncores.a $(ROOTDIR)/src/common/hardware.o

eard_monitor: eard_monitor.c
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c \
	$(ROOTDIR)/src/common/ear_daemon_client.o

freeipmi_overhead: freeipmi_overhead.c 
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) $(IPMI_CFLAGS) -o freeipmi_overhead freeipmi_overhead.c \
	$(PAPI_LDFLAGS) $(IPMI_LDFLAGS) $(ROOTDIR)/src/daemon/ear_node_energy_metrics.a \
	$(ROOTDIR)/src/common/hardware.o

simd_tests.o:simd_tests.c
	/opt/gcc/7.2.0/bin/gcc $(CFLAGS) -O0 -malign-double -march=native -c simd_tests.c

simd_power_metrics_openmp: simd_power_metrics_openmp.c simd_tests.o
	icc -qopenmp -axCORE_AVX512 $(CFLAGS) $(PAPI_CFLAGS) -o simd_power_metrics_openmp simd_power_metrics_openmp.c \
        $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) simd_tests.o \
	-I$(ROOTDIR)/src/library/ \
	$(ROOTDIR)/src/common/hardware.o \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o \
	$(ROOTDIR)/src/daemon/ear_frequency.o \
	$(ROOTDIR)/src/daemon/ear_turbo.o \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(ROOTDIR)/src/library/ear_metrics/ear_basic.o \
	$(ROOTDIR)/src/library/ear_metrics/ear_stalls.o

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
