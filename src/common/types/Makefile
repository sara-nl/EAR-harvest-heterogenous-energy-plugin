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

all: types.a

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

types.a: $(OBJS)
	$(AR) rvs ../types.a $(OBJS)

######## OPTIONS

clean:
	rm -f *.o *.a *.so
