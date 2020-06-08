CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

gpu_OBJS = \
    nvml.o

######## RULES

all: $(gpu_OBJS)

nvml.o: nvml.c nvml.h
	$(CC) $(CFLAGS) -I$(CUDA_BASE)/include -DCUDA_BASE=\"$(CUDA_BASE)\" -c $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
