API_DEBUG=
CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread $(API_DEBUG)
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(DB_LDFLAGS) -pthread

######## FILES

eard_OBJS += power_monitor.o \
             dynamic_configuration.o \
             shared_configuration.o \
             eard_server_api.o \
			 eard_rapi.o \
			 eard_checkpoint.o \
			 app_api/app_server_api.o

ifdef DB_BASE
eard_OBJS += $(SRCDIR)/common/database.a
endif

eard_OBJS += $(SRCDIR)/common/environment_common.o \
             $(SRCDIR)/common/sockets.o \
             $(SRCDIR)/common/types.a \
             $(SRCDIR)/control/frequency.o \
             $(SRCDIR)/database_cache/eardbd_api.o \
             $(SRCDIR)/metrics/libmetrics.a

BINARIES = eard_api.o \
           eard_rapi.o \
           eard_server_api.o \
           eard

######## RULES

all: app_api_bin $(BINARIES) all.others

eard: eard.c $(eard_OBJS) ../../Makefile 
	$(CC) $(PAPI_CFLAGS) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eard eard.c $(eard_OBJS) $(LDFLAGS)

power_monitor.o: power_monitor.c power_monitor.h 
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitor.c

dynamic_configuration.o: dynamic_configuration.c dynamic_configuration.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

shared_configuration.o: shared_configuration.c shared_configuration.h
	$(CC) -fPIC $(CFLAGS) -c shared_configuration.c

eard_rapi.o: eard_rapi.c eard_rapi.h
	$(CC) -fPIC $(CFLAGS) -c eard_rapi.c

eard_server_api.o: eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c eard_server_api.c 

eard_api.o: eard_api.c eard_api.h ../../Makefile
	$(CC) -fPIC $(CFLAGS) -c eard_api.c

eard_checkpoint.o:eard_checkpoint.c eard_checkpoint.h
	$(CC) -fPIC $(CFLAGS) -c eard_checkpoint.c

app_api_bin:
	$(MAKE) -C 'app_api' all

######## OPTIONS

install: install.others 
	cp eard $(DESTDIR)/sbin

devel.install: devel.install.others

%.install: ;

clean: clean.others
	rm -f *.o *.a *.so eard

######## SUBFOLDERS

%.others:
	$(MAKE) -C 'commands' $*
	$(MAKE) -C 'app_api' $*

######## DEPENDENCIES

depend: depend.others

depend-clean: depend-clean.others

include $(SRCDIR)/Makefile.depend
