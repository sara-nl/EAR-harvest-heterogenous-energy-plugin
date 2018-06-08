CFLAGS  = -I$(SRCDIR)
LDFLAGS =

######## FILES

extern_OBJS = $(SRCDIR)/common/types.a \
              $(SRCDIR)/global_manager/eargm_rapi.o

ifdef DB_BASE
BINARIES += eargm_new_job \
		    eargm_end_job
endif

######## RULES

all: $(BINARIES)

eargm_new_job: eargm_new_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o eargm_new_job eargm_new_job.c $(extern_OBJS)

eargm_end_job: eargm_end_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o eargm_end_job eargm_end_job.c $(extern_OBJS)

######## OPTIONS

install:

clean:
	rm -f $(BINARIES)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
