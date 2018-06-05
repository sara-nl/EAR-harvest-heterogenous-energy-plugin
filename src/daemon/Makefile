CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread 
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(DB_LDFLAGS) -pthread

######## VARS

export DIR

######## FILES

DAEMON_OBJS += power_monitor/libpowermon.a \
               dynamic_configuration.o \
               shared_configuration.o \
               eard_client_api.o \
               eard_server_api.o

ifdef DB_BASE
DAEMON_OBJS += $(SRCDIR)/common/database/db_helper.o \
               $(SRCDIR)/common/database/mysql_io_functions.o \
               $(SRCDIR)/common/string_enhanced.o
endif

DAEMON_OBJS += $(SRCDIR)/common/types.a \
               $(SRCDIR)/common/environment_common.o \
               $(SRCDIR)/control/frequency.o \
               $(SRCDIR)/metrics/libmetrics.a \
               $(SRCDIR)/database_cache/eardbd_api.o

HEADERS += $(SRCDIR)/daemon/eard_api_conf.h \
           $(SRCDIR)/daemon/eard_command_api_conf.h \
           $(SRCDIR)/daemon/shared_configuration.h \
           $(SRCDIR)/common/ear_verbose.h \
           $(SRCDIR)/common/states.h \
           $(SRCDIR)/common/config.h

BINARIES = eard_client_api.o \
           eard_server_api.o \
           eard_command_api.o \
           eard

######## RULES

.PHONY: components commands

all: all.components $(BINARIES) all.others

eard: eard.c $(DAEMON_OBJS) $(HEADERS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eard eard.c $(DAEMON_OBJS) $(LDFLAGS)

dynamic_configuration.o: dynamic_configuration.c eard_server_api.h $(HEADERS)
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c dynamic_configuration.c

shared_configuration.o: shared_configuration.c shared_configuration.h $(HEADERS)
	$(CC) -fPIC $(CFLAGS) -c shared_configuration.c

eard_server_api.o: eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c eard_server_api.c

eard_client_api.o: eard_client_api.c eard_client_api.h eard_api_conf.h
	$(CC) -fPIC $(CFLAGS) -c eard_client_api.c

eard_command_api.o: eard_command_api.c eard_command_api.h eard_api_conf.h
	$(CC) -fPIC $(CFLAGS) -c eard_command_api.c

######## OPTIONS

install: install.components install.others
	mv eard $(DESTDIR)/sbin

clean: clean.components clean.others
	rm -f *.o *.a *.so eard

######## SUBFOLDERS

%.components:
	$(MAKE) -C 'power_monitor' $*

%.others:
	$(MAKE) -C 'commands' $*
