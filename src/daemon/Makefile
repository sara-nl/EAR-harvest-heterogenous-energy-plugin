CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -DPOWER_MONITORING -pthread
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl -pthread

######## VARS

export DIR

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/bandwidth.so \
               $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/ipmi/energy_node.so \
               $(SRCDIR)/metrics/papi/energy_cpu.o

COMMON_OBJS = $(SRCDIR)/common/types.a
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

dynamic_configuration.o: dynamic_configuration.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

remote_daemon_api.o: remote_daemon_api.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c remote_daemon_api.c

eard: ear_daemon.c $(OBJS)
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(OBJS) $(LDFLAGS)

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so