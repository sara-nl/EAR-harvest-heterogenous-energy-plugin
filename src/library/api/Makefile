API_CFLAGS = $(CFLAGS) $(MPI_CCFLAGS) 

######## FILES

OBJS = \
    ear.o \
    ear_mpi.o \
    ear_default.o \
    mpic.o \
    mpif.o \
		ear_seq.o

######## RULES

all: $(OBJS) libear_seq.so
ifdef MPI_API
ear.o: ear.c ear.h
	$(MPICC) $(API_CFLAGS)  $(PAPI_CFLAGS) -c $<	
else
ear.o: ear.c ear.h
	$(CC) $(API_CFLAGS)  $(PAPI_CFLAGS) -c $<	
endif

%.o: %.c %.h
	$(CC) $(API_CFLAGS) -c $<

libear_seq.so:ear_seq.o
	$(CC) -shared $(PAPI_LDFLAGS) -ldl -o libear_seq.so ear_seq.o	

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
