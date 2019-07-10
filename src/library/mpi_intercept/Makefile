######## FILES

OBJS = ear_api.o \
       process_MPI.o \
       MPI_intercept.o \
       MPI_interface.o \
       MPI_interceptF.o \
       MPI_interfaceF.o

######## RULES

all: $(OBJS)

allt: DEFINES=-DEAR_GUI
allt: $(OBJS)

%.o: %.c %.h
	$(MPICC) $(CC_FLAGS) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
