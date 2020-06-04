CUDA_ROOT  = /hpc/base/cuda/cuda-10.1
CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -I$(CUDA_ROOT)/include

######## FILES

gpu_OBJS = \
    nvml.o

######## RULES

all: $(gpu_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
