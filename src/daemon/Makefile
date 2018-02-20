CFLAGS  = -I$(SRCDIR) $(CONSTANTS)
LDFLAGS = $(PAPI_LDFLAGS) $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) -ldl

######## VARS

export DIR

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/bandwidth/uncores_pci.o \
               $(SRCDIR)/metrics/custom/bandwidth.o \
               $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_act.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_wct.o \
               $(SRCDIR)/metrics/ipmi/energy_node.o \
	       $(SRCDIR)/metrics/papi/energy_cpu.o

COMMON_OBJS = $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/types.a \

BINARIES  = eard

######## RULES

all: eard

ear_frequency.o: ear_frequency.c ear_frequency.h
	$(CC) $(CFLAGS) $(CPUF_CFLAGS) $(PAPI_CFLAGS) -c ear_frequency.c

eard: ear_daemon.c ear_frequency.o
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(LDFLAGS) ear_frequency.o

######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
