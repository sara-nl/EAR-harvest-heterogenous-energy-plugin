DIR         = $(ROOTDIR)/tests/functionals
CFLAGS      = -I$(DIR) -I$(ROOTDIR)/src/common

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/hardware.o          \
             $(ROOTDIR)/src/common/environment.o       \
             $(ROOTDIR)/src/common/string_enhanced.o   \
             $(ROOTDIR)/src/common/ear_daemon_client.o

BINARIES = bandwith_monitor       \
           eard_monitor           \
           freeipmi_overhead      \
           simd_power_metrics_mpi \
           simd_power_metrics_pthreads

######## RULES

all: $(BINARIES)

eard_monitor: eard_monitor.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c $(OUTER_OBJS)

freeipmi_overhead: freeipmi_overhead.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) $(IPMI_CFLAGS) -o freeipmi_overhead freeipmi_overhead.c \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(IPMI_LDFLAGS) \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a

simd_metrics_pthreads: simd_power_metrics_pthreads.c $(OUTER_OBJS)
	/opt/gcc/7.2.0/bin/gcc $(CFLAGS) -march=native $(PAPI_CFLAGS) -o simd_metrics_pthreads simd_metrics_pthreads.c \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o                  \
        $(ROOTDIR)/src/daemon/ear_turbo.o

simd_power_metrics_avx512: simd_power_metrics_avx512.c $(OUTER_OBJS) 
	icc $(CFLAGS) -O0 -malign-double -axMIC-AVX512  $(PAPI_CFLAGS) -o simd_power_metrics_avx512 simd_power_metrics_avx512.c \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o                  \
        $(ROOTDIR)/src/daemon/ear_turbo.o                      \

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
