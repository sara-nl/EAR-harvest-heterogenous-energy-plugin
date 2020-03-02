SRCDIR  = ../..
CC      = /hpc/base/intel/compilers_and_libraries_2019.4.243/linux/mpi/intel64/bin/mpiicc
CFLAGS  = -Wall -I$(SRCDIR) -fPIC
LDFLAGS = -static-intel

######## FILES

OBJS = \
    mpic_interface.o \
    mpif_interface.o \
    ear_interface.o

LIBS = \
	libinterface.so

######## RULES

all: $(OBJS) $(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

libinterface.so:
	$(CC) -shared -o libinterface.so $(OBJS) $(LDFLAGS)


######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
