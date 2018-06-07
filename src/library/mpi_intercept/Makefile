######## FILES

OBJS = ear_api.o \
       process_MPI.o \
       MPI_intercept.o \
       MPI_interface.o

######## RULES

all: $(OBJS)

allt: $(eval DEFINES=-DEAR_GUI) $(OBJS)

%.o: %.c %.h
	$(MPICC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS 

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
