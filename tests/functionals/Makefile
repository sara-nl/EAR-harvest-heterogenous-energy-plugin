CFLAGS      = -I$(SRCDIR) $(PAPI_CFLAGS) $(IPMI_CFLAGS)

######## FILES

HARDWARE_OBJS = $(SRCDIR)/metrics/custom/hardware_info.o

BANDWIDTH_OBJS = $(SRCDIR)/metrics/custom/bandwidth.a

NODE_ENERGY_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.a

FREQUENCY_OBJS = $(SRCDIR)/metrics/custom/frequency.o \
                 $(SRCDIR)/daemon/ear_frequency.o


PAPI_OBJS = $(SRCDIR)/metrics/papi/flops.o \
            $(SRCDIR)/metrics/papi/instructions.o \
            $(SRCDIR)/metrics/papi/stalls.o \
            $(SRCDIR)/metrics/papi/energy_cpu.o

BINARIES = bandwith_monitor \
           eard_monitor \
           freeipmi_overhead \
           simd_power_metrics_openmp \
           energy_1_second \
           energy_updates_freq \
	   test_architecture

######## RULES

all: $(BINARIES)

bandwith_monitor: bandwith_monitor.c $(BANDWIDTH_OBJS) $(HARDWARE_OBJS)
	$(CC) $(CFLAGS) -o bandwith_monitor bandwith_monitor.c $(BANDWIDTH_OBJS) $(HARDWARE_OBJS)

eard_monitor: eard_monitor.c $(SRCDIR)/common/ear_daemon_client.o
	$(CC) $(CFLAGS) -o eard_monitor eard_monitor.c $(SRCDIR)/common/ear_daemon_client.o

freeipmi_overhead: freeipmi_overhead.c $(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)
	$(CC) $(CFLAGS) -o freeipmi_overhead freeipmi_overhead.c $(IPMI_LDFLAGS) $(PAPI_LDFLAGS) \
	$(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)

simd_tests.o: simd_tests.c
	gcc $(CFLAGS) -O0 -malign-double -march=native -c simd_tests.c

# CONVERT TO GCC PLEASE
simd_power_metrics_openmp: simd_power_metrics_openmp.c simd_tests.o
	icc -qopenmp -axCORE_AVX512 $(CFLAGS) -o simd_power_metrics_openmp simd_power_metrics_openmp.c \
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) simd_tests.o \
	$(HARDWARE_OBJS) $(NODE_ENERGY_OBJS) $(PAPI_OBJS) $(FREQUENCY_OBJS)

energy_1_second: energy_1_second.c $(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)
	gcc $(CFLAGS) -o energy_1_second energy_1_second.c $(IPMI_LDFLAGS) $(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)

energy_updates_freq: energy_updates_freq.c $(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)
	gcc $(CFLAGS) -o energy_updates_freq energy_updates_freq.c $(IPMI_LDFLAGS) $(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)

test_architecture: test_architecture.c
	gcc $(PAPI_CFLAGS) $(CFLAGS) -o test_architecture test_architecture.c \
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(NODE_ENERGY_OBJS) $(HARDWARE_OBJS)

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so
