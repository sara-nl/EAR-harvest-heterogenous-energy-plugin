SRCDIR = ../../..

######## RULES

all: $(arch_OBJS)
	/hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -Wall -fPIC -I $(SRCDIR) -DCONNECTOR=1 \
	-o test_conn connector_test.c \
	../connector.o ../cpufreq.o ../governor.o \
	../archs/amd17.o ../archs/dummy.o \
	../drivers/acpi_cpufreq.o \
	$(SRCDIR)/metrics/libmetrics.a $(SRCDIR)/common/libcommon.a \
	-lpthread -lm

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
