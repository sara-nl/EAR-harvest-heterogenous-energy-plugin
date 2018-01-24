DIR         = $(ROOTDIR)/tests/functionals
CFLAGS      = -I$(DIR) -I$(ROOTDIR)/src/common

######## FILES

BINARIES = bandwith_monitor       \
           eard_monitor           \
           freeipmi_overhead

######## RULES

all: $(BINARIES)

bandwith_monitor: bandwith_monitor.c
	$(CC) $(CFLAGS) -I$(ROOTDIR)/src/daemon -o bandwith_monitor bandwith_monitor.c \
	$(ROOTDIR)/src/daemon/ear_uncores.a $(ROOTDIR)/src/common/hardware.o

eard_monitor: eard_monitor.c
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c \
	$(ROOTDIR)/src/common/ear_daemon_client.o

freeipmi_overhead: freeipmi_overhead.c 
	$(CC) $(CFLAGS) -I$(ROOTDIR)/src/daemon $(PAPI_CFLAGS) $(IPMI_CFLAGS) -o freeipmi_overhead freeipmi_overhead.c \
	$(PAPI_LDFLAGS) $(IPMI_LDFLAGS) $(ROOTDIR)/src/daemon/ear_node_energy_metrics.a \
	$(ROOTDIR)/src/common/hardware.o

simd_metrics_mpi: simd_power_metrics_mpi.c
	$(MPICC) -O0 -axMIC-AVX412 $(CFLAGS) $(PAPI_CFLAGS) -o simd_metrics_mpi simd_metrics_pthreads.c \
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o \
	$(ROOTDIR)/src/daemon/ear_frequency.o    \
	$(ROOTDIR)/src/daemon/ear_turbo.o

simd_metrics_pthreads: simd_power_metrics_pthreads.c
	$(CC) -O0 -march=native $(CFLAGS) $(PAPI_CFLAGS) -o simd_metrics_pthreads simd_metrics_pthreads.c \
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) -lpthread $(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o \
	$(ROOTDIR)/src/daemon/ear_frequency.o    \
	$(ROOTDIR)/src/daemon/ear_turbo.o

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
