CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread
CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread -DPOWERMON_FREQ=30000000 
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl -pthread 
POWERMONLIB = $(SRCDIR)/metrics/power_monitoring/libpowermon.a

######## VARS

export DIR

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/frequency.o \
	           $(SRCDIR)/metrics/papi/generics.o \
	           $(SRCDIR)/control/frequency.o

INTERNAL_EAR_LIBS = $(SRCDIR)/metrics/custom/bandwidth.a \
				    $(SRCDIR)/common/types.a 

COMMON_OBJS = $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/shared_configuration.o

OBJS = power_monitoring.o \
       dynamic_configuration.o 

BINARIES  = eard

######## RULES

all: eard

eard: ear_daemon.c $(OBJS) $(METRICS_OBJS) $(POWERMONLIB) $(COMMON_OBJS) $(INTERNAL_EAR_LIBS)
	$(CC) $(CFLAGS) -g -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(LDFLAGS) $(OBJS) $(INTERNAL_EAR_LIBS) $(POWERMONLIB)

dynamic_configuration.o: dynamic_configuration.c 
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

power_monitoring.o: power_monitoring.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitoring.c

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
