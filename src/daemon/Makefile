CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread -DPOWERMON_FREQ=30000000
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -pthread

######## VARS

export DIR

######## FILES

COMMON_OBJS = $(SRCDIR)/metrics/power_monitoring/libpowermon.a \
              $(SRCDIR)/metrics/custom/bandwidth.a \
              $(SRCDIR)/metrics/custom/frequency.o \
	          $(SRCDIR)/metrics/papi/generics.o \
	          $(SRCDIR)/control/frequency.o \
              $(SRCDIR)/common/environment_common.o \
              $(SRCDIR)/common/shared_configuration.o \
              $(SRCDIR)/common/types.a 

ifdef DB_BASE
COMMON_OBJS += $(SRCDIR)/common/database/db_helper.o \
               $(SRCDIR)/common/database/mysql_io_functions.o 
endif

OBJS = power_monitoring.o \
       dynamic_configuration.o 

BINARIES  = eard

######## RULES

all: eard

eard: ear_daemon.c $(OBJS) $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eard ear_daemon.c $(LDFLAGS) $(DB_LDFLAGS) $(COMMON_OBJS) $(OBJS)

dynamic_configuration.o: dynamic_configuration.c 
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

power_monitoring.o: power_monitoring.c
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitoring.c

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
