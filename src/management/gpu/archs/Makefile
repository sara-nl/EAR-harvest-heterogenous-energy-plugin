CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -Wall

######## FILES

ifdef CUDA_BASE
gpu_OBJS = nvml.o
endif

######## RULES

all: $(gpu_OBJS)
#all: $(gpu_OBJS) tclock

nvml.o: nvml.c nvml.h
	$(CC) $(CFLAGS) -I$(CUDA_BASE)/include $(gpu_DEFS) -c $< -I /hpc/base/cuda/cuda-10.1/include/
	
#tclock: nvml.o test.c
#	$(CC) $(CFLAGS) -o tclock test.c nvml.o ../../../common/libcommon.a -L /hpc/base/cuda/cuda-10.1/lib64/stubs -lnvidia-ml -lpthread -ldl 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
