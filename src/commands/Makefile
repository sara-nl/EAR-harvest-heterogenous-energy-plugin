CFLAGS  = -I$(SRCDIR)
LDFLAGS =
CCC     = $(CC) $(CC_FLAGS) $(CFLAGS)

######## INSTALLATION

ifdef DB_BASE
G1_BINS = eacct econtrol ereport
G1_PATH = $(DESTDIR)/bin
G1_PERM = 2751
G2_BINS = edb_create edb_clean_pm
G2_PATH = $(DESTDIR)/sbin
G2_PERM = 0751
endif

######## FILES

erep_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o \
            $(SRCDIR)/common/types.a

econ_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
            $(SRCDIR)/common/types.a

edbc_OBJS = $(SRCDIR)/common/database/db_helper.o \
            $(SRCDIR)/common/database/mysql_io_functions.o \
            $(SRCDIR)/common/types.a

######## RULES

all: $(G1_BINS) $(G2_BINS)

eacct: ear_acct.c $(ereport_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(erep_OBJS) $(DB_LDFLAGS)

econtrol: ear_control.c $(econ_OBJS)
	$(CCC) -o $@ $< $(econ_OBJS)

ereport: ear_report.c $(ereport_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(erep_OBJS) $(DB_LDFLAGS)

edb_clean_pm: ear_cleandb.c $(ereport_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(edbc_OBJS) $(DB_LDFLAGS)

edb_create: ear_create_database.c $(ear_cleandb_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(edbc_OBJS) $(DB_LDFLAGS)

######## OPTIONS

install: G1.ginstall G2.ginstall

%.install: ;

clean: G1.gclean G2.gclean rclean

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
