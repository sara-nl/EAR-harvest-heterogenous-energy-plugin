CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

SRCS = cluster_conf.c \
       cluster_conf_read.c \
       cluster_conf_verbose.c \
	   policy_conf.c

######## RULES

all: cluster_conf.o

cluster_conf.o: $(SRCS) cluster_conf.h
	$(CC) $(CFLAGS) -c $(SRCS)

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
