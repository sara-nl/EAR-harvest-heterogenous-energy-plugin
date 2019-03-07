API_DEBUG =
CFLAGS    = -I$(SRCDIR) $(CONSTANTS) $(API_DEBUG) -O2
LDFLAGS   = $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(DB_LDFLAGS) -lm -pthread

######## FILES

eard_OBJS += app_api/app_server_api.o \
             power_monitor.o \
             dynamic_configuration.o \
             shared_configuration.o \
             eard_checkpoint.o \
             eard_server_api.o \
             eard_rapi.o

ifdef DB_BASE
eard_OBJS += $(SRCDIR)/common/database.a
endif

eard_OBJS += $(SRCDIR)/common/environment_common.o \
             $(SRCDIR)/common/sockets.o \
             $(SRCDIR)/common/types.a \
             $(SRCDIR)/control/frequency.o \
             $(SRCDIR)/database_cache/eardbd_api.o \
             $(SRCDIR)/metrics/libmetrics.a

BINS = eard_api.o \
       eard_rapi.o \
       eard_server_api.o \
       eard

######## RULES

all: all.components $(BINS) all.others

eard: eard.c $(eard_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(eard_OBJS) $(LDFLAGS)

power_monitor.o: power_monitor.c power_monitor.h 
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c $<

dynamic_configuration.o: dynamic_configuration.c dynamic_configuration.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c $<

shared_configuration.o: shared_configuration.c shared_configuration.h
	$(CC) -fPIC $(CFLAGS) -c $<

eard_rapi.o: eard_rapi.c eard_rapi.h
	$(CC) -fPIC $(CFLAGS) -c $<

eard_server_api.o: eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c $< 

eard_api.o: eard_api.c eard_api.h ../../Makefile
	$(CC) -fPIC $(CFLAGS) -c $<

eard_checkpoint.o: eard_checkpoint.c eard_checkpoint.h
	$(CC) -fPIC $(CFLAGS) -c $<

######## OPTIONS

install: install.components install.others
	cp eard $(DESTDIR)/sbin
	chmod 750 $(DESTDIR)/sbin/eard

%.install: %.install.components %.install.others;

clean: clean.components clean.others
	rm -f *.o *.a *.so eard

######## SUBFOLDERS

%.components:
	$(MAKE) -C 'app_api' $*

%.others:
	$(MAKE) -C 'commands' $*

######## DEPENDENCIES

depend: depend.components depend.others

depend-clean: depend-clean.components depend-clean.others

include $(SRCDIR)/Makefile.depend
