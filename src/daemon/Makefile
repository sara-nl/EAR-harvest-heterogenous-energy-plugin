CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -DPOWER_MONITORING -pthread
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl -pthread

######## VARS

export DIR

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/bandwidth.a \
               $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/ipmi/energy_node.a \
	       $(SRCDIR)/metrics/papi/energy_cpu.o

COMMON_OBJS = $(SRCDIR)/common/environment.o \
			  $(SRCDIR)/common/shared_configuration.o \
              $(SRCDIR)/common/types.a \

BINARIES  = eard

######## RULES

all: eard

ear_frequency.o: ear_frequency.c ear_frequency.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) $(PAPI_CFLAGS) -c ear_frequency.c

eard: ear_daemon.c ear_frequency.o power_monitoring.o dynamic_configuration.o remote_daemon_api.o
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(LDFLAGS) ear_frequency.o power_monitoring.o \
		dynamic_configuration.o remote_daemon_api.o


dynamic_configuration.o:dynamic_configuration.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c
remote_daemon_api.o:remote_daemon_api.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c remote_daemon_api.c
######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
