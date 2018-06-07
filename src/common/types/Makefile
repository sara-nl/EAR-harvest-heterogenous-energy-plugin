CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

OBJS = application.o \
       coefficient.o \
       job.o \
       log.o \
       loop.o \
       periodic_metric.o \
       power_signature.o \
       projection.o \
       signature.o \
       gm_warning.o \
       cluster_conf.o

######## RULES

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
