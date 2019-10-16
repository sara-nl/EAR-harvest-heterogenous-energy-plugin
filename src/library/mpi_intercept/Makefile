######## FILES

OBJS = \
    ear_api.o \
    process_MPI.o \
    mpi_definitions.o \
    mpi_interception.o \
    MPI_interface.o

ifeq ($(FEAT_FORT), 1)
OBJS += \
    MPI_interceptF.o \
    MPI_interfaceF.o
endif

######## RULES

all: $(OBJS)

allt: DEFINES=-DEAR_GUI
allt: $(OBJS)

%.o: %.c %.h
	$(MPICC) $(CC_FLAGS) $(CFLAGS) -c $<

ear_api.o: ear_api.c ear_api.h
	$(MPICC) $(CC_FLAGS) $(CFLAGS) -DSHOW_DEBUGS -c ear_api.c

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
