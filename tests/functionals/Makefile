DIR    = $(ROOTDIR)/tests/functionals
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/daemon/ear_uncores.a       \
             $(ROOTDIR)/src/common/ear_arch_type.o     \
             $(ROOTDIR)/src/common/ear_daemon_client.o \
             $(ROOTDIR)/src/common/ear_configuration.o

BINARIES = daemon_monitoring \
           intel_pci_uncores_stress

######## RULES

all: $(BINARIES)

intel_pci_uncores_stress: intel_pci_uncores_stress.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o intel_pci_uncores_stress intel_pci_uncores_stress.c $(OUTER_OBJS)

daemon_monitoring: daemon_monitoring.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o daemon_monitoring daemon_monitoring.c $(OUTER_OBJS)

######## OPTIONS

clean:
	rm -f $(BINARIES)
	rm -f *.o
