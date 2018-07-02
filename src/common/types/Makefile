CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

OBJS = application.o \
       coefficient.o \
       job.o \
       log.o \
       loop.o \
       periodic_aggregation.o \
       periodic_metric.o \
       power_signature.o \
       projection.o \
       signature.o \
       gm_warning.o \
       services.o

######## RULES

all: all.components $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: install.components

clean: clean.components
	rm -f *.o *.a *.so

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'configuration' $* || exit

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
