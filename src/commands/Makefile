CFLAGS  = -I$(SRCDIR)
LDFLAGS =
CCC     = $(CC) $(CC_FLAGS) $(CFLAGS)

######## INSTALLATION

ifdef DB_BASE
com1_BINS = eacct econtrol ereport
com1_PATH = $(DESTDIR)/bin
com1_PERM = 2751
com2_BINS = edb_create edb_clean_pm
com2_PATH = $(DESTDIR)/sbin
com2_PERM = 0751
endif

######## FILES

comm_OBJS = $(SRCDIR)/common/libcommon.a

######## RULES

all: $(com1_BINS) $(com2_BINS)

eacct: ear_acct.c $(comm_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(comm_OBJS) $(DB_LDFLAGS)

econtrol: ear_control.c $(comm_OBJS)
	$(CCC) -o $@ $< $(comm_OBJS)

ereport: ear_report.c $(comm_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(comm_OBJS) $(DB_LDFLAGS)

edb_clean_pm: ear_cleandb.c $(comm_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(comm_OBJS) $(DB_LDFLAGS)

edb_create: ear_create_database.c $(comm_OBJS)
	$(CCC) $(DB_CFLAGS) -o $@ $< $(comm_OBJS) $(DB_LDFLAGS)

######## OPTIONS

install: com1.ginstall com2.ginstall

%.install: ;

clean: com1.gclean com2.gclean rclean

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
