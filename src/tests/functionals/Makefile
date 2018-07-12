CFLAGS = -I$(SRCDIR) $(PAPI_CFLAGS) $(IPMI_CFLAGS)

######## FILES

BINARIES = bandwith_monitor \
           eard_monitor \
           freeipmi_overhead \
           simd_power_metrics_openmp \
           energy_1_second \
           energy_updates_freq \
		   ear_conf \
		   my_node_conf \
		   configuration_check \
		   batch_insert
# ear_pm
# test_architecture

######## RULES

all: $(BINARIES)

batch_insert: batch_insert.c \
	$(SRCDIR)/common/types.a \
	$(SRCDIR)/common/database.a
	$(CC) $(CC_FLAGS) $(DB_LDFLAGS) $(CFLAGS) -o batch_insert $^ $(SRCDIR)/common/types.a $(SRCDIR)/common/database.a

configuration_check: configuration_check.c $(SRCDIR)/common/types.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -o configuration_check $^ $(SRCDIR)/common/types.a

my_node_conf: my_node_conf.c $(SRCDIR)/common/types.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -o my_node_conf  $^

ear_conf: ear_conf.c $(SRCDIR)/common/types.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -o ear_conf $^

bandwith_monitor: bandwith_monitor.c \
        $(SRCDIR)/metrics/custom/hardware_info.o \
        $(SRCDIR)/metrics/custom/bandwidth.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -o bandwith_monitor $^

eard_monitor: eard_monitor.c \
        $(SRCDIR)/daemon/eard_api.o \
        $(SRCDIR)/common/types.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -o eard_monitor $^

freeipmi_overhead: freeipmi_overhead.c \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -o freeipmi_overhead $^ $(IPMI_LDFLAGS) $(PAPI_LDFLAGS)

simd_tests.o: simd_tests.c
	$(CC) $(CC_FLAGS) $(CFLAGS) -O0 -malign-double -march=native -c simd_tests.c

simd_power_metrics_openmp: simd_power_metrics_openmp.c \
        $(SRCDIR)/metrics/libmetrics.a \
        $(SRCDIR)/control/frequency.o\
        simd_tests.o
	$(CC) $(CC_FLAGS) -fopenmp -malign-double -march=native $(CFLAGS) -o simd_power_metrics_openmp $^ \
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS)

energy_1_second: energy_1_second.c \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -o energy_1_second $^ $(IPMI_LDFLAGS)

energy_updates_freq: energy_updates_freq.c \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -o energy_updates_freq $^ $(IPMI_LDFLAGS)

test_architecture: test_architecture.c \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/hardware_info.o
	$(CC) $(CC_FLAGS) $(PAPI_CFLAGS) $(CFLAGS) -o test_architecture $^ \
	$(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS)

ear_pm: ear_pm.c \
        $(SRCDIR)/daemon/power_monitoring/libpowermon.a \
        $(SRCDIR)/metrics/ipmi/energy_node.a \
        $(SRCDIR)/metrics/custom/frequency.o
	$(CC) $(CC_FLAGS) $(CFLAGS) $(CPUF_CFLAGS) -o ear_pm $^ $(IPMI_LDFLAGS) $(PAPI_LDFLAGS)

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
