CFLAGS  = -I$(SRCDIR)
LDFLAGS = $(DB_LDFLAGS) -lpthread

######## FILES

HEADERS = $(SRCDIR)/common/config.h \
          $(SRCDIR)/daemon/eard_command_api.h \
          $(SRCDIR)/daemon/eard_command_api_conf.h

DAEMON_OBJS = $(SRCDIR)/common/types.a \
              eargm_server_api.o

OBJS = eargm_client_api.o \
       eargm_server_api.o

ifdef DB_BASE
BINARIES += eargmd

DB_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o
endif

######## RULES

all: $(OBJS) $(BINARIES) all.others

eargmd: eargm.c $(DAEMON_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o eargmd eargm.c $(DAEMON_OBJS) $(LDFLAGS)

eargm_client_api.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c eargm_client_api.c

eargm_server_api.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c eargm_server_api.c

######## OPTIONS

install: install.others
	cp $(BINARIES) $(DESTDIR)/sbin

clean: clean.others
	rm -rf $(BINARIES)

depend: depend.others

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

######## SUBFOLDERS

%.others:
	$(MAKE) -C 'commands' $*