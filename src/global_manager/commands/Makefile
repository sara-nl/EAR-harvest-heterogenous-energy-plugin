CFLAGS  = -I$(SRCDIR)
LDFLAGS =

######## INSTALLATION

ifdef DB_BASE
eard_BINS += eargm_new_job eargm_end_job
eard_BINS = eargmd
eard_PATH = $(DESTDIR)/sbin
eard_PERM = 0750
endif

######## FILES

extern_OBJS = $(SRCDIR)/common/types.a \
    $(SRCDIR)/global_manager/eargm_rapi.o

######## RULES

all: $(eard_BINS)

eargm_new_job: eargm_new_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(extern_OBJS)

eargm_end_job: eargm_end_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(extern_OBJS)

######## OPTIONS

install: ;

devel.install: eard.ginstall;

%.install: ;

clean: eard.clean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
