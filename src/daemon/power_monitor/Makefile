CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## OBJS

COMMON_OBJS = $(SRCDIR)/common/math_operations.o

lib_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.o \
           $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_ina226.o \
           $(SRCDIR)/metrics/custom/hardware_info.o \
           $(SRCDIR)/metrics/papi/energy_cpu.o

OBJS = power_monitor.o \
       power_monitor_api.o

######## RULES

all: libpowermon.a libpowermon.so

power_monitor.o: power_monitor.c power_monitor.h
	$(CC) $(CFLAGS) -c power_monitor.c

power_monitor_api.o: power_monitor_api.c power_monitor_api.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitor_api.c

libpowermon.a: $(OBJS) $(lib_OBJS) $(COMMON_OBJS)
	$(AR) rcs libpowermon.a $(OBJS) $(lib_OBJS) $(COMMON_OBJS)

libpowermon.so: $(OBJS) $(lib_OBJS) $(COMMON_OBJS)
	$(CC) -shared $(OBJS) $(lib_OBJS) $(COMMON_OBJS) -o libpowermon.so

######## OPTIONS

install: install.others
	cp libpowermon.so $(DESTDIR)/lib

install.others:
	mkdir -p $(DESTDIR)/include/daemon/power_monitor
	cp power_monitor.h $(DESTDIR)/include/daemon/power_monitor

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
