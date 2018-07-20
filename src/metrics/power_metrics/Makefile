CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## OBJS

COMMON_OBJS = $(SRCDIR)/common/math_operations.o

lib_OBJS = $(SRCDIR)/metrics/ipmi/energy_node.o \
           $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
           $(SRCDIR)/metrics/ipmi/energy_node/lenovo_sd650.o \
           $(SRCDIR)/metrics/custom/hardware_info.o \
           $(SRCDIR)/metrics/papi/energy_cpu.o

OBJS = power_metrics.o 

######## RULES

all: libpowermetrics.a libpowermetrics.so

power_metrics.o: power_metrics.c power_metrics.h
	$(CC) $(CFLAGS) -c power_metrics.c

libpowermetrics.a: $(OBJS) $(lib_OBJS) $(COMMON_OBJS)
	$(AR) rcs libpowermetrics.a $(OBJS) $(lib_OBJS) $(COMMON_OBJS)

libpowermetrics.so: $(OBJS) $(lib_OBJS) $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -shared $(OBJS) $(lib_OBJS) $(COMMON_OBJS) -o libpowermetrics.so

######## OPTIONS

install: 
	cp libpowermetrics.so $(DESTDIR)/lib
	mkdir -p $(DESTDIR)/include/metrics/power_metrics
	cp power_metrics.h $(DESTDIR)/include/metrics/power_metrics

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
