API_DEBUG=
CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread $(API_DEBUG)
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(DB_LDFLAGS) -pthread

######## FILES

eard_OBJS += power_monitor.o \
             dynamic_configuration.o \
             shared_configuration.o \
             eard_server_api.o \
			 eard_conf.o\
             eard_api.o
ifdef DB_BASE
eard_OBJS += $(SRCDIR)/common/database.a
endif
eard_OBJS += $(SRCDIR)/common/types.a \
             $(SRCDIR)/common/environment_common.o \
             $(SRCDIR)/control/frequency.o \
             $(SRCDIR)/metrics/libmetrics.a \
             $(SRCDIR)/database_cache/eardbd_api.o

BINARIES = eard_api.o \
           eard_rapi.o \
           eard_server_api.o \
           eard

######## RULES

all: $(BINARIES) all.others

eard: eard.c $(eard_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eard eard.c $(eard_OBJS) $(LDFLAGS)

power_monitor.o: power_monitor.c power_monitor.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitor.c

dynamic_configuration.o: dynamic_configuration.c dynamic_configuration.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

shared_configuration.o: shared_configuration.c shared_configuration.h
	$(CC) -fPIC $(CFLAGS) -c shared_configuration.c

eard_server_api.o: eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c eard_server_api.c

eard_api.o: eard_api.c eard_api.h
	$(CC) -fPIC $(CFLAGS) -c eard_api.c

eard_rapi.o: eard_rapi.c eard_rapi.h
	$(CC) -fPIC $(CFLAGS) -c eard_rapi.c

eard_conf.o:eard_conf.c eard_conf.h
	$(CC) -fPIC $(CFLAGS) -c eard_conf.c

######## OPTIONS

install: install.others
	cp eard $(DESTDIR)/sbin

clean: clean.others
	rm -f *.o *.a *.so eard

depend: depend.others

depend-clean: depend-clean.others

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

######## SUBFOLDERS

%.others:
	$(MAKE) -C 'commands' $*

