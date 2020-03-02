SRCDIR  = ../..
CC      = /hpc/base/intel/compilers_and_libraries_2019.4.243/linux/mpi/intel64/bin/mpiicc
CFLAGS  = -Wall -I$(SRCDIR) -fPIC
LDFLAGS = -static-intel

######## FILES

OBJS = \
    loader.o \
    mpic_syms.o \
    mpif_syms.o

LIBS = \
	libloader.so

######## RULES

all: $(OBJS) $(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

libloader.so:
	$(CC) -shared -o libloader.so $(OBJS) $(LDFLAGS) ../../common/libcommon.a

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
