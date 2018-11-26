CFLAGS  = -I$(SRCDIR)
LDFLAGS =

######## FILES

ereport_OBJS  = $(SRCDIR)/common/database/mysql_io_functions.o \
                $(SRCDIR)/common/types.a

econtrol_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
                $(SRCDIR)/common/types.a

ear_cleandb_OBJS =	$(SRCDIR)/common/database/db_helper.o \
					$(SRCDIR)/common/database/mysql_io_functions.o \
					$(SRCDIR)/common/types.a

ifdef DB_BASE
BINS  = eacct econtrol ereport
SBINS = ear_create_database ear_cleandb
endif

######## RULES

all: $(BINS) $(SBINS)

eacct: ear_acct.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ereport_OBJS)

econtrol: ear_control.c $(econtrol_OBJS) 
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(econtrol_OBJS) 

ereport: ear_report.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ereport_OBJS)

ear_cleandb: ear_cleandb.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ear_cleandb_OBJS)

ear_create_database: ear_create_database.c $(SRCDIR)/common/types.a
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(SRCDIR)/common/types.a

######## OPTIONS

install:
ifdef DB_BASE
	cp $(BINS) $(DESTDIR)/bin/
	cp $(SBINS) $(DESTDIR)/sbin/
endif

%.install: ;

clean:
	rm -f $(BINS) $(SBINS)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
