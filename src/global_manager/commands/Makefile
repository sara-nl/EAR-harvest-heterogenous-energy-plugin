API_DEBUG=
LDFLAGS =
CFLAGS  = -I$(SRCDIR) $(API_DEBUG)

######## FILES

extern_OBJS = $(SRCDIR)/common/types.a \
              $(SRCDIR)/global_manager/eargm_rapi.o

ifdef DB_BASE
BINS += eargm_new_job \
        eargm_end_job
endif

######## RULES

all: $(BINS)

eargm_new_job: eargm_new_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(extern_OBJS)

eargm_end_job: eargm_end_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(extern_OBJS)

######## OPTIONS

install:

devel.install: ;
	cp $(BINS) $(DESTDIR)/sbin

%.install: ;

clean:
	rm -f $(BINS)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
