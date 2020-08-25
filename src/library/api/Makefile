######## FILES

OBJS = \
    ear.o \
    ear_mpi.o \
    ear_seq.o \
    mpic.o \
    mpif.o \

BINS = libear_seq.so

######## RULES

all: $(OBJS) $(BINS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(MPI_CFLAGS) $(CFLAGS) -c $<

libear_seq.so: ear_seq.o
	$(CC) $(CC_FLAGS) -shared -o $@ $^ -ldl

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
