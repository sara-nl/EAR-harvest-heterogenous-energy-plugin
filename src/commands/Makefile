CFLAGS  = -I$(SRCDIR)
LDFLAGS =

######## FILES

ereport_OBJS  = $(SRCDIR)/common/database/mysql_io_functions.o \
                $(SRCDIR)/common/types.a

econtrol_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
                $(SRCDIR)/common/types.a

ear_cleandb_OBJS = $(SRCDIR)/common/database/db_helper.o \
                   $(SRCDIR)/common/database/mysql_io_functions.o \
                   $(SRCDIR)/common/types.a

ifdef DB_BASE
G1_BINS = eacct econtrol ereport
G1_PATH = $(DESTDIR)/bin
G1_PERM = 2751

G2_BINS = edb_create edb_clean_pm
G2_PATH = $(DESTDIR)/sbin
G2_PERM = 0751
endif

######## RULES

all: $(G1_BINS) $(G2_BINS)

eacct: ear_acct.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ereport_OBJS)

econtrol: ear_control.c $(econtrol_OBJS) 
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(econtrol_OBJS) 

ereport: ear_report.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ereport_OBJS)

edb_clean_pm: ear_cleandb.c $(ereport_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ear_cleandb_OBJS)

edb_create: ear_create_database.c $(ear_cleandb_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(DB_LDFLAGS) $(ear_cleandb_OBJS)

######## OPTIONS

install: G1.ginstall G2.ginstall

%.install: ;

clean: rclean G1.gclean G2.gclean

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
