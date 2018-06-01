CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## OBJS

COMMON_OBJS = $(SRCDIR)/daemon/eard_api.o \
              $(SRCDIR)/common/math_operations.o

LIB_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.o \
           $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_ina226.o \
           $(SRCDIR)/metrics/papi/energy_cpu.o  \
           $(SRCDIR)/metrics/custom/hardware_info.o

######## RULES

all: libpowermon.a libpowermon.so

libpowermon.a: ear_power_monitor.o $(LIB_OBJS) $(COMMON_OBJS)
	ar rcs libpowermon.a ear_power_monitor.o $(LIB_OBJS) $(COMMON_OBJS)

ear_power_monitor.o: ear_power_monitor.c ear_power_monitor.h $(SRCDIR)/daemon/eard_api.h
	$(CC) $(CFLAGS) -c ear_power_monitor.c 

libpowermon.so: ear_power_monitor.o $(LIB_OBJS)
	$(CC) -shared ear_power_monitor.o $(LIB_OBJS) -o libpowermon.so


######## OPTIONS

install:
	mkdir -p $(DESTDIR)/include/metrics/power_monitoring
	cp ear_power_monitor.h $(DESTDIR)/include/metrics/power_monitoring
	cp libpowermon.so $(DESTDIR)/lib

clean:
	rm -f *.so *.a *.o
