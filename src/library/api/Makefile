API_CFLAGS = $(CFLAGS) $(MPI_CFLAGS) 

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

ear.o: ear.c ear.h
	$(CC) $(API_CFLAGS)  $(PAPI_CFLAGS) -c $<	

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
