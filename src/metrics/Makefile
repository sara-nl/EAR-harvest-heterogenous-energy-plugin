CFLAGS  = -fPIC -shared

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/papi/energy_cpu.o \
               $(SRCDIR)/metrics/papi/generics.o \
               $(SRCDIR)/metrics/papi/cache.o \
               $(SRCDIR)/metrics/papi/flops.o \
               $(SRCDIR)/metrics/papi/instructions.o \
               $(SRCDIR)/metrics/papi/stalls.o \
               $(SRCDIR)/metrics/ipmi/energy_node.o \
               $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_nm.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_ina226.o \
               $(SRCDIR)/metrics/custom/bandwidth.o \
               $(SRCDIR)/metrics/custom/bandwidth/uncores_pci.o

######## RULES

all: all.components libmetrics.a libmetrics.so

libmetrics.a: $(METRICS_OBJS)
	$(AR) rvs libmetrics.a $(METRICS_OBJS)

libmetrics.so: $(METRICS_OBJS)
	$(CC) $(CFLAGS) -o libmetrics.so $(METRICS_OBJS)

######## OPTIONS

install: install.others install.components

install.others:
	mkdir -p $(DESTDIR)/include/metrics/custom
	mkdir -p $(DESTDIR)/include/metrics/ipmi
	mkdir -p $(DESTDIR)/include/metrics/papi
	cp metrics.h $(DESTDIR)/include/metrics
	cp libmetrics.so $(DESTDIR)/lib/libmetrics.so

clean: clean.components
	rm -f *.o *.a *.so

depend: depend.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'custom' $* || exit
	@ $(MAKE) -C 'ipmi' $* || exit
	@ $(MAKE) -C 'papi' $* || exit
