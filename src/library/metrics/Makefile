CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -O2 -fPIC

########

TARGETS = \
	metrics.o

ifdef CUDA_BASE
TARGETS += \
		gpu.o
endif

all: \
	$(TARGETS)

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
