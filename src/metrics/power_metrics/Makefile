CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## INSTALLATION

libs_BINS = libpowermetrics.a libpowermetrics.so
libs_OBJS = \
    $(SRCDIR)/metrics/finder/energy.o \
    $(SRCDIR)/metrics/handler/energy.o \
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
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) $(libs_OBJS) $(COMMON_OBJS)

######## OPTIONS

install: ;

devel.install: ; 

%.install: ;

clean: libs.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
