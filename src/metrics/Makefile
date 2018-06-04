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

COMPONENTS = custom \
             ipmi \
             papi \

######## RULES

all: components libmetrics.a libmetrics.so

components:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} all || exit; \
	done

libmetrics.a: $(METRICS_OBJS)
	ar rvs libmetrics.a $(METRICS_OBJS)

libmetrics.so: $(METRICS_OBJS)
	$(CC) $(CFLAGS) -o libmetrics.so $(METRICS_OBJS)

######## OPTIONS

install: component-installs other-installs

component-installs:
	@ for i in $(COMPONENTS); do \
		mkdir -p $(DESTDIR)/include/metrics/$${i} ; \
	done
	@ for i in $(COMPONENTS); do \
		$(MAKE) -S -C $${i} install; \
	done

other-installs:
	mv libear_metrics.so $(DESTDIR)/lib/libear_metrics.so
	cp metrics.h $(DESTDIR)/include/metrics

clean:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} clean; \
	done
	rm -rf *.so *.a *.o
