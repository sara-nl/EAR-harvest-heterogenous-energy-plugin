CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## OBJS

COMMON_OBJS = $(SRCDIR)/common/math_operations.o

LIB_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.o \
           $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_ina226.o \
           $(SRCDIR)/metrics/custom/hardware_info.o \
           $(SRCDIR)/metrics/papi/energy_cpu.o

OBJS = power_monitor.o \
       power_monitor_api.o

######## RULES

all: libpowermon.a libpowermon.so

power_monitor.o: power_monitor.c power_monitor.h $(HEADERS)
	$(CC) $(CFLAGS) -c power_monitor.c

power_monitor_api.o: power_monitor_api.c power_monitor_api.h $(HEADERS)
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) -c power_monitor_api.c

libpowermon.a: $(OBJS) $(LIB_OBJS) $(COMMON_OBJS)
	ar rcs libpowermon.a $(OBJS) $(LIB_OBJS) $(COMMON_OBJS)

libpowermon.so: $(OBJS) $(LIB_OBJS) $(COMMON_OBJS)
	$(CC) -shared $(OBJS) $(LIB_OBJS) $(COMMON_OBJS) -o libpowermon.so

######## OPTIONS

install: install-others
	cp libpowermon.so $(DESTDIR)/lib

install-others:
	mkdir -p $(DESTDIR)/include/daemon/power_monitoring
	cp ear_power_monitor.h $(DESTDIR)/include/daemon/power_monitoring

clean:
	rm -f *.so *.a *.o
