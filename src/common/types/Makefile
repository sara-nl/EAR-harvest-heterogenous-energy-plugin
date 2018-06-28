CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

CONF_OBJS = all.components configuration/cluster_conf.o

OBJS = application.o \
       coefficient.o \
       job.o \
       log.o \
       loop.o \
       periodic_metric.o \
       power_signature.o \
       projection.o \
       signature.o \
       gm_warning.o

######## RULES

all: all.components $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

types.o: $(OBJS) $(CONF_OBJS)

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'configuration' $* || exit

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
