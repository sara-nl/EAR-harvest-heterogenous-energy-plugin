CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -O2 -fPIC

########

all: \
        gpu.o \
        metrics.o 

%.o: %.c %.h
	$(MPICC) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

depend: ;

depend-clean: ;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
