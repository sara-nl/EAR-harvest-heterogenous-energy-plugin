CFLAGS = $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) -O2 -fPIC

######## FILES

OBJS = \
    ear.o \
    ear_mpi.o \
    api_mpic.o \
    api_mpif.o \

######## RULES

all: $(OBJS)

api_ear.o: api_ear.c api_ear.h
	$(CC) $(CFLAGS)  $(PAPI_CFLAGS) -c $<	

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
