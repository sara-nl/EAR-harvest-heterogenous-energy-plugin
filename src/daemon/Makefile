CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -DPOWER_MONITORING -DEARDS
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl -pthread 

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/power_monitoring/ear_power_monitor.o \
               $(SRCDIR)/metrics/custom/bandwidth.a \
               $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/ipmi/energy_node.a \
	           $(SRCDIR)/metrics/papi/energy_cpu.o \
	           $(SRCDIR)/metrics/papi/generics.o \
	           $(SRCDIR)/control/frequency.o

COMMON_OBJS = $(SRCDIR)/common/types.a \
              $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/shared_configuration.o

OBJS = power_monitoring.o \
       dynamic_configuration.o \
       remote_daemon_api.o

BINARIES  = eard

######## RULES

all: eard

eard: ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(OBJS)
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(OBJS) $(LDFLAGS)

dynamic_configuration.o: dynamic_configuration.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

remote_daemon_api.o: remote_daemon_api.c $(SRCDIR)/common/remote_conf.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c remote_daemon_api.c

power_monitoring.o:power_monitoring.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitoring.c

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
