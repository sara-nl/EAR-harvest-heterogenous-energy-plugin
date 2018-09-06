CFLAGS  = -I$(SRCDIR)
LDFLAGS = $(DB_LDFLAGS) -lpthread

######## FILES

daemon_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
              $(SRCDIR)/common/database.a \
              $(SRCDIR)/common/types.a \
              eargm_server_api.o

OBJS = eargm_rapi.o \
       eargm_server_api.o

BINARIES = eargmd

######## RULES

all: $(OBJS) $(BINARIES) all.others 

eargmd: eargm.c $(daemon_OBJS) $(DB_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eargmd eargm.c $(LDFLAGS) $(DB_OBJS) $(daemon_OBJS) 

%.o: %.c %.h
	$(CC) -fPIC $(CFLAGS) -c $<

######## OPTIONS

install: install.others
	cp $(BINARIES) $(DESTDIR)/sbin

devel.install: devel.install.others

%.install: ;

clean: clean.others
	rm -f $(BINARIES)
	rm -f *.o *.a *.so

######## SUBFOLDERS

%.others:
	@ $(MAKE) -C 'commands' $* || exit

######## DEPENDENCIES

depend: depend.others

depend-clean: depend-clean.others

include $(SRCDIR)/Makefile.depend
