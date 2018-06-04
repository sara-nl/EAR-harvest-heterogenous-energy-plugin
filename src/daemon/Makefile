CFLAGS  = -I$(SRCDIR) $(CONSTANTS) -pthread 
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -pthread

######## VARS

export DIR

######## FILES

ifdef DB_BASE
COMMON_OBJS += $(SRCDIR)/common/database/db_helper.o \
               $(SRCDIR)/common/database/mysql_io_functions.o \
               $(SRCDIR)/common/string_enhanced.o
endif

COMMON_OBJS += $(SRCDIR)/common/environment_common.o \
               $(SRCDIR)/common/types.a

OUTER_OBJS += $(SRCDIR)/database_cache/eardbd_api.o \
              $(SRCDIR)/metrics/libmetrics.a \
              $(SRCDIR)/control/frequency.o

OBJS += power_monitor/libpowermon.a \
        dynamic_configuration.o \
        shared_configuration.o \
        eard_client_api.o \
        eard_server_api.o

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

all: components $(BINARIES) commands

components:
	$(MAKE) -C power_monitor all

commands:
	$(MAKE) -C commands all

eard: eard.c $(OBJS) $(COMMON_OBJS) $(OUTER_OBJS) $(HEADERS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eard eard.c $(OBJS) $(COMMON_OBJS) $(OUTER_OBJS) $(LDFLAGS) $(DB_LDFLAGS)

dynamic_configuration.o: dynamic_configuration.c $(HEADERS) eard_server_api.h
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

install: install-subfolders
	mv eard $(DESTDIR)/sbin

install-subfolders:
	$(MAKE) -C commands install
	$(MAKE) -C power_monitor install

clean: clean-subfolders
	rm -f *.o *.a *.so

clean-subfolders:
	$(MAKE) -C commands clean
	$(MAKE) -C power_monitor clean
