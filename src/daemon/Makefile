CFLAGS    = -I$(SRCDIR) $(CONSTANTS)
LDFLAGS   = $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(DB_LDFLAGS) -lm -pthread

######## INSTALLATION

eard_BINS = eard
eard_PATH = $(DESTDIR)/sbin
eard_PERM = 0750
oths_BINS = eard_api.o

######## FILES

eard_OBJS = eard_checkpoint.o \
        eard_rapi.o \
        eard_server_api.o \
        dynamic_configuration.o \
        power_monitor.o \
        shared_configuration.o \
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

######## RULES

all: all.components $(eard_BINS) $(oths_BINS) all.others

eard: eard.c $(eard_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(eard_OBJS) $(LDFLAGS)

power_monitor.o: power_monitor.c power_monitor.h 
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c $<

dynamic_configuration.o: dynamic_configuration.c dynamic_configuration.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c $<

shared_configuration.o: shared_configuration.c shared_configuration.h
	$(CC) $(CFLAGS) -fPIC -c $<

eard_api.o: eard_api.c eard_api.h
	$(CC) $(CFLAGS) -fPIC -c $<

eard_rapi.o: eard_rapi.c eard_rapi.h
	$(CC) $(CFLAGS) -fPIC -c $<

eard_server_api.o: eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c $< 

eard_checkpoint.o: eard_checkpoint.c eard_checkpoint.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: install.components install.others eard.ginstall;

%.install: %.install.components %.install.others;

clean: clean.components clean.others eard.gclean rclean;

######## SUBFOLDERS

%.components:
	$(MAKE) -C 'app_api' $*

%.others:
	$(MAKE) -C 'commands' $*

######## DEPENDENCIES

depend: depend.components depend.others

depend-clean: depend-clean.components depend-clean.others

include $(SRCDIR)/Makefile.depend
