CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## INSTALLATION

libs_BINS = libpowermetrics.a libpowermetrics.so
libs_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.o \
    $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
    $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
    $(SRCDIR)/metrics/ipmi/energy_node/lenovo_sd650.o \
    $(SRCDIR)/metrics/custom/hardware_info.o \
    $(SRCDIR)/metrics/papi/energy_cpu.o
libs_PATH = $(DESTDIR)/lib
libs_PERM = 0660

incl_BINS = power_metrics.h
incl_PATH = $(DESTDIR)/include
incl_PERM = 0660

######## OBJS

COMMON_OBJS = $(SRCDIR)/common/math_operations.o
OBJS = power_metrics.o 

######## RULES

all: libpowermetrics.a libpowermetrics.so

power_metrics.o: power_metrics.c power_metrics.h
	$(CC) $(CFLAGS) -c $<

libpowermetrics.a: $(OBJS) $(libs_OBJS) $(COMMON_OBJS)
	$(AR) rcs $@ $(OBJS) $(libs_OBJS) $(COMMON_OBJS)

libpowermetrics.so: $(OBJS) $(libs_OBJS) $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -shared -o $@ $(OBJS) $(libs_OBJS) $(COMMON_OBJS)

######## OPTIONS

install:

devel.install: libs.ginstall incl.ginstall; 

%.install: ;

clean: libs.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra