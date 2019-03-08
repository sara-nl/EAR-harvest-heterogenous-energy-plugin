######## RULES

all: tracer.o tracer_mpi.o

allt: DEFINES=-DEAR_GUI
allt: tracer.o tracer_mpi.o

%.o: %.c %.h
	$(MPICC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
