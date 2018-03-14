CFLAGS  = -I$(SRCDIR) $(CONSTANTS)  -pthread
CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -DPOWER_MONITORING -pthread 
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl -pthread -L$(SRCDIR)/metrics/power_monitoring -lpowermon

METRICS_DAEMON = $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/bandwidth.o \
               $(SRCDIR)/metrics/custom/bandwidth/uncores_pci.o 

POWERMONLIB = $(SRCDIR)/metrics/power_monitoring/libpowermon.a


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


eard: ear_daemon.c ear_frequency.o power_monitoring.o dynamic_configuration.o remote_daemon_api.o $(METRICS_DAEMON) $(POWERMONLIB)
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_DAEMON) $(LDFLAGS) ear_frequency.o power_monitoring.o \
		dynamic_configuration.o remote_daemon_api.o

ear_frequency.o: ear_frequency.c ear_frequency.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) $(PAPI_CFLAGS) -c ear_frequency.c

dynamic_configuration.o:dynamic_configuration.c
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
