SRCDIR = ../../..

######## RULES

all: $(arch_OBJS)
	/hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -Wall -fPIC -I $(SRCDIR) \
	-o test test.c $(SRCDIR)/metrics/libmetrics.a $(SRCDIR)/common/libcommon.a -lpthread -lm

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
