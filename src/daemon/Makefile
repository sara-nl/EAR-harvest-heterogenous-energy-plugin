CFLAGS  = -I$(SRCDIR) $(CONSTANTS) 
LDFLAGS = $(CPUF_LDFLAGS) $(IPMI_LDFLAGS) $(PAPI_LDFLAGS) -ldl 

######## FILES

METRICS_OBJS = $(SRCDIR)/metrics/custom/bandwidth.a \
               $(SRCDIR)/metrics/custom/frequency.o \
               $(SRCDIR)/metrics/custom/hardware_info.o \
               $(SRCDIR)/metrics/ipmi/energy_node.a \
	           $(SRCDIR)/metrics/papi/energy_cpu.o \
	           $(SRCDIR)/metrics/papi/generics.o \
	           $(SRCDIR)/control/frequency.o

COMMON_OBJS = $(SRCDIR)/common/environment.o \
              $(SRCDIR)/common/types.a \

BINARIES  = eard

######## RULES

all: eard

eard: ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS)
	$(CC) $(CFLAGS) -o eard ear_daemon.c $(COMMON_OBJS) $(METRICS_OBJS) $(LDFLAGS)


######## OPTIONS

install:
	mv eard $(DESTDIR)/sbin

clean:
	rm -f *.o *.a *.so
