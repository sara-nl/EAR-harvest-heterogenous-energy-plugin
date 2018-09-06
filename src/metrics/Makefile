CFLAGS  = -fPIC -shared

######## FILES

metrics_OBJS = $(SRCDIR)/metrics/common/msr.o \
               $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/frequency_uncore.o \
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
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_sd650.o \
               $(SRCDIR)/metrics/custom/bandwidth.o \
               $(SRCDIR)/metrics/custom/bandwidth/uncores_pci.o \
               $(SRCDIR)/metrics/power_metrics/power_metrics.o

######## RULES

all: all.components libmetrics.a libmetrics.so

libmetrics.a: $(metrics_OBJS)
	$(AR) rvs libmetrics.a $(metrics_OBJS)

libmetrics.so: $(metrics_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o libmetrics.so $(metrics_OBJS)

######## OPTIONS

install: install.components

devel.install: devel.install.components;
	cp metrics.h $(DESTDIR)/include/metrics/
	cp libmetrics.so $(DESTDIR)/lib/

%.install: ;

clean: clean.components
	rm -f *.o *.a *.so

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'common' $* || exit
	@ $(MAKE) -C 'custom' $* || exit
	@ $(MAKE) -C 'ipmi' $* || exit
	@ $(MAKE) -C 'papi' $* || exit
	@ $(MAKE) -C 'power_metrics' $* || exit
