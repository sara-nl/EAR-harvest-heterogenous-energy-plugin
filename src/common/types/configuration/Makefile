CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

OBJS = cluster_conf.o \
       cluster_conf_read.o \
       cluster_conf_verbose.o \
	   policy_conf.o

######## RULES

all: $(OBJS)

%.o: %.c cluster_conf.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

%.install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
