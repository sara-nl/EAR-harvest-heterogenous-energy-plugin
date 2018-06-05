CFLAGS  = -fPIC -shared

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/custom/bandwidth.a \
			   $(SRCDIR)/metrics/ipmi/energy_node.a
               $(SRCDIR)/metrics/papi/energy_cpu.o \
               $(SRCDIR)/metrics/papi/generics.o \
               $(SRCDIR)/metrics/papi/cache.o \
               $(SRCDIR)/metrics/papi/flops.o \
               $(SRCDIR)/metrics/papi/instructions.o \
               $(SRCDIR)/metrics/papi/stalls.o

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
	mv libmetrics.so $(DESTDIR)/lib/libmetrics.so
	cp metrics.h $(DESTDIR)/include/metrics

clean: clean.components
	rm -rf *.so *.a *.o

######## SUBFOLDERS

%.components:
	$(MAKE) -C 'custom' $*
	$(MAKE) -C 'ipmi' $*
	$(MAKE) -C 'papi' $*