SRCDIR = ../../..
MPICC  = /hpc/base/intel/compilers_and_libraries_2019.4.243/linux/mpi/intel64/bin/mpiicc
MPICC_FLAGS = -I$(SRCDIR) -static-intel -Werror-all

######## FILES

OBJS = \
    mpi_defs.o \
    mpic_syms.o \
    mpif_syms.o \
    mpic_syms_ear.o \
    mpif_syms_ear.o \
    mpic_syms_void.o \
    mpif_syms_void.o

######## RULES

all: $(OBJS)

%.o: %.c %.h
	$(MPICC) -Wall $(MPICC_FLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
