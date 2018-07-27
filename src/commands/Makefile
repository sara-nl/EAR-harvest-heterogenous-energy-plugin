CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## FILES

erport_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o \
              $(SRCDIR)/common/types.a

econtrol_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
				$(SRCDIR)/common/types.a	

ifdef DB_BASE
BINARIES = eacct econtrol

SBINARIES += ear_create_database 
endif

######## RULES

all: $(BINARIES) $(SBINARIES) $(COMPONENTS)

eacct: ear_acct.c $(erport_OBJS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o eacct ear_acct.c $(DB_LDFLAGS) $(erport_OBJS)

econtrol: ear_control.c $(econtrol_OBJS) 
	$(CC) $(CFLAGS) -o econtrol ear_control.c $(econtrol_OBJS) 

ear_create_database: ear_create_database.c $(SRCDIR)/common/types.a
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS) $(SRCDIR)/common/types.a

ear_energy_over_time: ear_energy_over_time.c
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_energy_over_time ear_energy_over_time.c $(DB_LDFLAGS)

ear_store_database: ear_store_database.c
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_store_database ear_store_database.c $(DB_LDFLAGS)

######## OPTIONS

install:
ifdef DB_BASE
	@ cp $(BINARIES) $(DESTDIR)/bin/
	@ cp $(SBINARIES) $(DESTDIR)/sbin/
endif

clean:
	rm -f $(BINARIES) $(SBINARIES)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
