CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -Wall
SRCDIR = /home/xjaneas/ngit/ear_private/src

######## FILES

psta_BINS = \
    cpufreq.o \
    cpufreq_user.o \
    connector.o

######## RULES

all: all.components $(psta_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

#	$(CC) $(CFLAGS) -o pstate pstate.o archs/amd17.o drivers/cpufreq.o $(SRCDIR)/common/libcommon.a $(SRCDIR)/metrics/libmetrics.a -lpthread -lm

proto:
	/hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -Wall -fPIC -I $(SRCDIR) \
	-DCONNECTOR=1 -o test_conn connector_test.c \
	connector.o cpufreq.o archs/amd17.o archs/dummy.o drivers/acpi_cpufreq.o \
	../../metrics/libmetrics.a ../../common/libcommon.a \
	-lpthread -lm

#	/hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -Wall -fPIC -I $(SRCDIR) \
#	-o test_freq connector_test.c \
#	pstate.o archs/amd17.o archs/dummy.o drivers/cpufreq.o \
#	../../metrics/libmetrics.a ../../common/libcommon.a \
#	-lpthread -lm

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'archs' $* || exit
	@ $(MAKE) -C 'drivers' $* || exit

######## OPTIONS

install: install.components

%.install: ;

clean: rclean clean.components
	rm -f test_freq
	rm -f test_conn

depend: depend.components

depend-clean: depend-clean.components

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
