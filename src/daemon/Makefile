CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread -DPOWERMON_FREQ=10000000
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -pthread

######## VARS

export DIR

######## FILES

COMMON_OBJS = 
OBJS =
HEADERS =

ifdef DB_BASE
COMMON_OBJS += $(SRCDIR)/common/database/db_helper.o \
               $(SRCDIR)/common/database/mysql_io_functions.o \
			   $(SRCDIR)/common/string_enhanced.o
endif

ifdef FEAT_SHRMEM
COMMON_OBJS += $(SRCDIR)/metrics/power_monitoring/libpowermon.a 

OBJS += dynamic_configuration.o \
        power_monitoring.o \
	    eard_server_api.o \
	    shared_configuration.o

EARD_API= eard_api.o \
		  eard_rapi.o

HEADERS += $(SRCDIR)/daemon/eard_conf_rapi.h \
           $(SRCDIR)/daemon/shared_configuration.h \
           $(SRCDIR)/metrics/power_monitoring/ear_power_monitor.h
endif

COMMON_OBJS += $(SRCDIR)/metrics/metrics.a \
	           $(SRCDIR)/control/frequency.o \
               $(SRCDIR)/common/environment_common.o \
               $(SRCDIR)/common/types.a 

HEADERS += $(SRCDIR)/daemon/eard_conf_api.h \
           $(SRCDIR)/common/ear_verbose.h \
           $(SRCDIR)/common/states.h \
           $(SRCDIR)/common/config.h

BINARIES = eard

######## RULES

all: eard $(EARD_API) 
	$(MAKE) -C commands all

eard: ear_daemon.c $(OBJS) $(COMMON_OBJS) $(HEADERS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eard ear_daemon.c $(OBJS) $(COMMON_OBJS) $(LDFLAGS) $(DB_LDFLAGS)

dynamic_configuration.o: dynamic_configuration.c power_monitoring.h $(HEADERS) eard_server_api.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

power_monitoring.o: power_monitoring.c power_monitoring.h $(HEADERS)
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitoring.c

eard_server_api.o:eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c eard_server_api.c

eard_api.o:eard_api.c eard_api.h eard_conf_api.h
	$(CC) -fPIC $(CFLAGS) -c eard_api.c

eard_rapi.o:eard_rapi.c eard_rapi.h eard_conf_rapi.h
	$(CC) -fPIC $(CFLAGS) -c eard_rapi.c 

shared_configuration.o: shared_configuration.c shared_configuration.h $(HEADERS)
	$(CC) -fPIC $(CFLAGS) -c shared_configuration.c


######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin
	$(MAKE) -C commands install

clean:
	rm -f *.o *.a *.so
	$(MAKE) -C commands clean
