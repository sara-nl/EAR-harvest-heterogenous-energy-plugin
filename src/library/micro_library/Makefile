LDFLAGS = -shared $(CPUF_LDFLAGS)

######## FILES

inn_OBJS = micro.o \
    process_MPI.o \
    MPI_intercept.o \
    MPI_interface.o
    #MPI_interceptF.o \
    #MPI_interfaceF.o

ext_OBJS = ../dynais/dynais_old.o \
    ../../common/libcommon.a

######## RULES

all: libear.so

allt: DEFINES=-DEAR_GUI
allt: $(OBJS)

%.o: %.c %.h
	$(MPICC) -Wall $(CFLAGS) $(DEFINES) -c $<

libear.so: $(inn_OBJS)
	$(MPICC) -Wall $(MPICC_FLAGS) $(CFLAGS) -o libear.so $(inn_OBJS) $(ext_OBJS) $(LDFLAGS)

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
