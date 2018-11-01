CFLAGS = -Wformat -fPIC -I$(SRCDIR) $(CONSTANTS)

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
       projection_old.o \
       signature.o \
       gm_warning.o \
       services.o

######## RULES

all: all.components $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: install.components

%.install: %.install.components;

clean: clean.components
	rm -f *.o *.a *.so

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'configuration' $* || exit

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend
