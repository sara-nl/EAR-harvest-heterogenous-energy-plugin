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
			   $(SRCDIR)/metrics/ipmi/energy_node.a \
			   $(SRCDIR)/metrics/custom/bandwidth.a
 

COMPONENTS = custom \
             ipmi \
             papi 

######## RULES

all:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} all || exit; \
	done 
	$(CC) $(CFLAGS) -o libear_metrics.so $(METRICS_OBJS)

######## OPTIONS

install:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -S -C $${i} install; \
	done
	mv libear_metrics.so $(DESTDIR)/lib/libear_metrics.so
	cp metrics.h $(DESTDIR)/include

clean:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} clean; \
	done
