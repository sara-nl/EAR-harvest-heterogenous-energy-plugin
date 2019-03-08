######## FILES

OBJS = ear_api.o \
       process_MPI.o \
       MPI_intercept.o \
       MPI_interface.o \
       MPI_interceptF.o \
       MPI_interfaceF.o \
       freq_synchro.o

######## RULES

all: $(OBJS)

allt: DEFINES=-DEAR_GUI
allt: $(OBJS)

%.o: %.c %.h
	$(MPICC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
