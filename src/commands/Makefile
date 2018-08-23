CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## FILES

ereport_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o \
              $(SRCDIR)/common/types.a

econtrol_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
				$(SRCDIR)/common/types.a	

ifdef DB_BASE
BINARIES = eacct econtrol ereport

SBINARIES += ear_create_database 
endif

######## RULES

all: $(BINARIES) $(SBINARIES) $(COMPONENTS)

eacct: ear_acct.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eacct ear_acct.c $(DB_LDFLAGS) $(ereport_OBJS)

econtrol: ear_control.c $(econtrol_OBJS) 
	$(CC) $(CC_FLAGS) $(CFLAGS) -o econtrol ear_control.c $(econtrol_OBJS) 

ear_create_database: ear_create_database.c $(SRCDIR)/common/types.a
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS) $(SRCDIR)/common/types.a

ereport: ear_report.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o ereport ear_report.c $(DB_LDFLAGS) $(ereport_OBJS)

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
