CFLAGS  = -I$(SRCDIR)
LDFLAGS = $(DB_LDFLAGS) -lpthread

######## FILES

manager_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
               $(SRCDIR)/common/database.a \
               $(SRCDIR)/common/types.a \
               eargm_server_api.o

OBJS = eargm_rapi.o \
       eargm_server_api.o

BINS = eargmd

######## RULES

all: $(OBJS) $(BINS) all.others 

eargmd: eargm.c $(manager_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $< $(manager_OBJS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) -fPIC $(CFLAGS) -c $<

######## OPTIONS

install: install.others
	cp eargmd $(DESTDIR)/sbin
	chmod 750 $(DESTDIR)/sbin/eargmd

devel.install: devel.install.others;

%.install: ;

clean: clean.others
	rm -f $(BINS)
	rm -f *.o *.a *.so

######## SUBFOLDERS

%.others:
	@ $(MAKE) -C 'commands' $* || exit

######## DEPENDENCIES

depend: depend.others

depend-clean: depend-clean.others

include $(SRCDIR)/Makefile.depend
