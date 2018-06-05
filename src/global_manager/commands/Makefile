CFLAGS  =
LDFLAGS =

######## FILES

HEADERS = $(SRCDIR)/common/types/cluster_conf.h

EXTERN_OBJS = $(SRCDIR)/common/types.a \
              $(SRCDIR)/commands/eargm_client_api.o

ifdef DB_BASE
BINARIES += eargm_new_job \
		    eargm_end_job
endif

######## RULES

all: $(BINARIES)

eargm_new_job: %.c $(EXTERN_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o eargm_new_job eargm_new_job.c $(EXTERN_OBJS)

eargm_end_job: %.c $(EXTERN_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o eargm_end_job eargm_end_job.c $(EXTERN_OBJS)

######## OPTIONS

install:

clean:
	rm -f $(BINARIES)
