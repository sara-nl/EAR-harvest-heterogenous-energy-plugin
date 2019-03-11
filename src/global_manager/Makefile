CFLAGS  = -I$(SRCDIR)
LDFLAGS = $(DB_LDFLAGS) -lpthread

######## INSTALLATION

eard_BINS = eargmd
eard_PATH = $(DESTDIR)/sbin
eard_PERM = 0750

######## FILES

comm_BINS = $(SRCDIR)/daemon/eard_rapi.o \
    $(SRCDIR)/common/libcommon.a \
    eargm_server_api.o

eard_OBJS = eargm_rapi.o \
    eargm_server_api.o

######## RULES

all: $(eard_OBJS) $(eard_BINS) all.others

eargmd: eargm.c $(comm_BINS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(comm_BINS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -fPIC -c $<

######## OPTIONS

install: install.others eard.ginstall;

devel.install: devel.install.others;

clean: clean.others eard.gclean rclean;

######## SUBFOLDERS

%.others:
	@ $(MAKE) -C 'commands' $* || exit

######## DEPENDENCIES

depend: depend.others

depend-clean: depend-clean.others

include $(SRCDIR)/Makefile.depend
