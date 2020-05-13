CFLAGS = $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) -O2 -fPIC -DMPI=1

######## FILES

OBJS = \
    ear.o \
    ear_mpi.o \
    mpic.o \
    mpif.o \

######## RULES

all: $(OBJS)

ear.o: ear.c ear.h
	$(CC) $(CFLAGS)  $(PAPI_CFLAGS) -c $<	

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
