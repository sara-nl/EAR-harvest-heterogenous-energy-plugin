######## FILES

OBJS = ear_api.o \
       process_MPI.o \
       MPI_intercept_empty.o \
       MPI_intercept_new.o \
       MPI_interface.o

######## RULES

all: $(OBJS)

allt: DEFINES=-DEAR_GUI
allt: $(OBJS)

%.o: %.c %.h
	$(MPICC) $(CC_FLAGS) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
