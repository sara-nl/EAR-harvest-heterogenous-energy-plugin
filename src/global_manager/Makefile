CFLAGS  = -I$(SRCDIR)
LDFLAGS = $(DB_LDFLAGS) -lpthread

######## FILES

daemon_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
              $(SRCDIR)/common/types.a \
              eargm_server_api.o

OBJS = eargm_rapi.o \
       eargm_server_api.o

ifdef DB_BASE
BINARIES += eargmd

DB_OBJS = $(SRCDIR)/common/database.a
endif

######## RULES

all: $(OBJS) $(BINARIES) all.others 

eargmd: eargm.c $(daemon_OBJS) $(DB_OBJS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o eargmd eargm.c $(LDFLAGS) $(DB_OBJS) $(daemon_OBJS) 

%.o: %.c %.h
	$(CC) -fPIC $(CFLAGS) -c $<

######## OPTIONS

install: install.others
	cp $(BINARIES) $(DESTDIR)/sbin

clean: clean.others
	rm -f $(BINARIES)
	rm -f *.o *.a *.so

depend: depend.others

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

######## SUBFOLDERS

%.others:
	@ $(MAKE) -C 'commands' $* || exit
