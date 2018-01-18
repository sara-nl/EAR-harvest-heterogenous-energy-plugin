DIR         = $(ROOTDIR)/tests/functionals
CFLAGS      = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon -I$(ROOTDIR)/src/library

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/hardware.o          \
             $(ROOTDIR)/src/common/environment.o       \
             $(ROOTDIR)/src/common/string_enhanced.o   \
             $(ROOTDIR)/src/common/ear_daemon_client.o \
             $(ROOTDIR)/src/library/ear_dynais/ear_dynais.o

BINARIES = intel_pci_uncores_stress \
           eard_monitor             \
           papi_cpu_gflops          \
           simd_power_metrics       \
           simd_power_metrics_2       \
           simd_power_metrics_sse       \
           simd_power_metrics_avx2       \
           freeipmi_overhead        \
           hardware_scanner         \
           dynais_input  \
	  simd_test_with_ear

######## RULES

all: $(BINARIES)

simd_test_with_ear:simd_test_with_ear.c
	mpiicc $(CFLAGS) -O0 -malign-double -vec-threshold0 -xCORE-AVX512 -qopt-report=5  simd_test_with_ear.c -o simd_test_with_ear

intel_pci_uncores_stress: intel_pci_uncores_stress.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o intel_pci_uncores_stress intel_pci_uncores_stress.c \
	$(OUTER_OBJS) $(ROOTDIR)/src/daemon/ear_uncores.a

eard_monitor: eard_monitor.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c $(OUTER_OBJS)

papi_cpu_gflops: papi_cpu_gflops.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_cpu_gflops papi_cpu_gflops.c \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) -lpthread

simd_power_metrics: simd_power_metrics.c $(OUTER_OBJS)
	/opt/gcc/7.2.0/bin/gcc $(CFLAGS) -march=native $(PAPI_CFLAGS) -o simd_power_metrics simd_power_metrics.c \
        $(ROOTDIR)/src/daemon/ear_turbo.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o           \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o        \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread

simd_power_metrics_2: simd_power_metrics_2.c $(OUTER_OBJS) 
	icc $(CFLAGS) -O0 -malign-double -axMIC-AVX512  $(PAPI_CFLAGS) -o simd_power_metrics_2 simd_power_metrics_2.c \
        $(ROOTDIR)/src/daemon/ear_turbo.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o           \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o        \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread dummy.c
freeipmi_overhead: freeipmi_overhead.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) $(IPMI_CFLAGS) -o freeipmi_overhead freeipmi_overhead.c \
	$(ROOTDIR)/src/daemon/ear_node_energy_metrics.a $(OUTER_OBJS) $(PAPI_LDFLAGS) $(IPMI_LDFLAGS)

simd_power_metrics_sse: simd_power_metrics_sse.c $(OUTER_OBJS) 
	icc $(CFLAGS) -O0 -malign-double -axSSE4.2  $(PAPI_CFLAGS) -o simd_power_metrics_sse simd_power_metrics_sse.c \
        $(ROOTDIR)/src/daemon/ear_turbo.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o           \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o        \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread dummy.c

hardware_scanner: hardware_scanner.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o hardware_scanner hardware_scanner.c $(OUTER_OBJS)

simd_power_metrics_avx512: simd_power_metrics_avx512.c $(OUTER_OBJS) 
	icc $(CFLAGS) -O0 -malign-double -axMIC-AVX512  $(PAPI_CFLAGS) -o simd_power_metrics_avx512 simd_power_metrics_avx512.c \
        $(ROOTDIR)/src/daemon/ear_turbo.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o           \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o        \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread dummy.c

dynais_input: dynais_input.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o dynais_input dynais_input.c $(OUTER_OBJS)

simd_power_metrics_avx2: simd_power_metrics_avx2.c $(OUTER_OBJS) 
	icc $(CFLAGS) -O0 -malign-double -axCORE-AVX2  $(PAPI_CFLAGS) -o simd_power_metrics_avx2 simd_power_metrics_avx2.c \
        $(ROOTDIR)/src/daemon/ear_turbo.o               \
        $(ROOTDIR)/src/daemon/ear_frequency.o           \
	$(ROOTDIR)/src/daemon/ear_rapl_metrics.o        \
	$(ROOTDIR)/src/library/ear_metrics/ear_flops_metrics.o \
	$(OUTER_OBJS) $(PAPI_LDFLAGS) $(SIMD_CFLAGS) $(CPUF_LDFLAGS) -lpthread dummy.c
######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
