GCFLAGS = $(CFLAGS) $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) -O2 -fPIC

OBJS = library_shared_data.o

ifeq ($(FEAT_MPI_API), 1)
OBJS += \
    global_comm.o
endif

######## RULES

all: $(OBJS)

%.o: %.c %.h
	$(MPICC) $(CC_FLAGS) $(GCFLAGS) -c $<

library_shared_data.o:library_shared_data.c 
	$(MPICC) $(MPICC_FLAGS) $(GCFLAGS) -c $< 	

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
