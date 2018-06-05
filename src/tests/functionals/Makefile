CFLAGS = -I$(SRCDIR) $(PAPI_CFLAGS) $(IPMI_CFLAGS)

######## FILES

HARDWARE_OBJS = $(SRCDIR)/metrics/custom/hardware_info.o

BANDWIDTH_OBJS = $(SRCDIR)/metrics/custom/bandwidth.a

NODE_ENERGY_LIBS = $(SRCDIR)/metrics/ipmi/energy_node.a

FREQUENCY_OBJS = $(SRCDIR)/metrics/custom/frequency.o \
                 $(SRCDIR)/control/frequency.o

TYPES = $(SRCDIR)/common/types.a

AVGFREQ_OBJS = $(SRCDIR)/metrics/custom/frequency.o

PAPI_OBJS = $(SRCDIR)/metrics/papi/flops.o \
            $(SRCDIR)/metrics/papi/instructions.o \
            $(SRCDIR)/metrics/papi/stalls.o \
            $(SRCDIR)/metrics/papi/energy_cpu.o \
            $(SRCDIR)/metrics/papi/generics.o

NODE_ENERGY_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.o \
			$(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
		    $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
			$(SRCDIR)/metrics/ipmi/energy_node/lenovo_ina226.o

POWERMONLIB = $(SRCDIR)/daemon/power_monitoring/libpowermon.a

BINARIES = bandwith_monitor \
           eard_monitor \
           freeipmi_overhead \
           simd_power_metrics_openmp \
           energy_1_second \
           energy_updates_freq
# ear_pm
# test_architecture

######## RULES

all: $(BINARIES)

bandwith_monitor: bandwith_monitor.c \
        $(SRCDIR)/metrics/custom/hardware_info.o \
        $(SRCDIR)/metrics/custom/bandwidth.a
	$(CC) $(CFLAGS) -o bandwith_monitor $@

eard_monitor: eard_monitor.c \
        $(SRCDIR)/daemon/eard_client_api.o
        $(SRCDIR)/common/types.a
	$(CC) $(CFLAGS) -o eard_monitor $@

freeipmi_overhead: freeipmi_overhead.c \
        $(SRCDIR)/metrics/ipmi/energy_node.a
	$(CC) $(CFLAGS) -o freeipmi_overhead $@ $(IPMI_LDFLAGS) $(PAPI_LDFLAGS)

simd_tests.o: simd_tests.c
	$(CC) $(CFLAGS) -O0 -malign-double -march=native -c simd_tests.c

simd_power_metrics_openmp: simd_power_metrics_openmp.c \
        $(SRCDIR)/metrics/libmetrics.a \
        $(SRCDIR)/control/frequency.o
        simd_tests.o \
	$(CC) -fopenmp -malign-double -march=native $(CFLAGS) -o simd_power_metrics_openmp $@
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS)

energy_1_second: energy_1_second.c \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o \
	$(CC) $(CFLAGS) -o energy_1_second $@ $(IPMI_LDFLAGS)

energy_updates_freq: energy_updates_freq.c
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o \
	$(CC) $(CFLAGS) -o energy_updates_freq $@ $(IPMI_LDFLAGS)

test_architecture: test_architecture.c
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o \
	$(CC) $(PAPI_CFLAGS) $(CFLAGS) -o test_architecture $@ $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS)

ear_pm: ear_pm.c \
        $(SRCDIR)/daemon/power_monitoring/libpowermon.a \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/frequency.o \
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -o ear_pm $@ $(IPMI_LDFLAGS) $(PAPI_LDFLAGS)

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so
