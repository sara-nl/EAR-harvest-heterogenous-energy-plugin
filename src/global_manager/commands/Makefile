CFLAGS  = -I$(SRCDIR)
LDFLAGS =

######## INSTALLATION

ifdef DB_BASE
gmdc_BINS += eargm_new_job eargm_end_job
gmdc_BINS = eargmd
gmdc_PATH = $(DESTDIR)/sbin
gmdc_PERM = 0750
endif

######## FILES

extern_OBJS = $(SRCDIR)/common/libcommon.a \
    $(SRCDIR)/global_manager/eargm_rapi.o

######## RULES

all: $(gmdc_BINS)

eargm_new_job: eargm_new_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(extern_OBJS)

eargm_end_job: eargm_end_job.c $(extern_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(extern_OBJS)

######## OPTIONS

install: ;

devel.install: gmdc.ginstall;

%.install: ;

clean: gmdc.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra