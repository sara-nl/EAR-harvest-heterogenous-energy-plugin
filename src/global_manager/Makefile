CFLAGS  = $(CC_FLAGS) -I$(SRCDIR) 
LDFLAGS = $(DB_LDFLAGS) -lpthread

######## INSTALLATION

eargm_BINS = eargmd
eargm_PATH = $(DESTDIR)/sbin
eargm_PERM = 0770

######## FILES

comm_BINS = $(SRCDIR)/daemon/eard_rapi.o \
    $(SRCDIR)/common/libcommon.a 

eargm_OBJS = eargm_rapi.o \
    eargm_server_api.o \
		cluster_powercap.o		

######## RULES

all: $(eargm_OBJS) $(eargm_BINS) all.others

eargmd: eargm.c $(comm_BINS) $(eargm_OBJS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(eargm_OBJS) $(comm_BINS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -fPIC -c $<

######## OPTIONS

install: install.others eargm.ginstall;

%.install: %.install.others;

clean: clean.others eargm.gclean rclean;

######## SUBFOLDERS

%.others: ;

######## DEPENDENCIES

depend: depend.others

depend-clean: depend-clean.others

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
