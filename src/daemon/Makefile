CFLAGS  = -I$(SRCDIR) $(CONSTANTS)  -pthread
CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -DPOWER_MONITORING -pthread -DNO_EARDS
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl -pthread 

METRICS_DAEMON = $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
                $(SRCDIR)/metrics/papi/energy_cpu.o \
               $(SRCDIR)/metrics/ipmi/energy_node.o \
               $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_act.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_wct.o \
               $(SRCDIR)/metrics/custom/bandwidth.o \
               $(SRCDIR)/metrics/custom/bandwidth/uncores_pci.o \
			   $(SRCDIR)/metrics/power_monitoring/ear_power_monitor.o


######## VARS

export DIR

######## FILES


COMMON_OBJS = $(SRCDIR)/common/types.a \
              $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/shared_configuration.o

OBJS = ear_frequency.o \
       power_monitoring.o \
       dynamic_configuration.o \
       remote_daemon_api.o

BINARIES  = eard

######## RULES

all: eard

ear_frequency.o: ear_frequency.c ear_frequency.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) $(PAPI_CFLAGS) -c ear_frequency.c

eard: ear_daemon.c ear_frequency.o power_monitoring.o dynamic_configuration.o remote_daemon_api.o $(METRICS_DAEMON)
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_DAEMON) $(LDFLAGS) ear_frequency.o power_monitoring.o \
		dynamic_configuration.o remote_daemon_api.o


dynamic_configuration.o:dynamic_configuration.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

remote_daemon_api.o: remote_daemon_api.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c remote_daemon_api.c
power_monitoring.o:power_monitoring.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitoring.c

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
