CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

ifdef IPMI_BASE
ener_BINS = \
    ipmi_ibm.so \
    ipmi_lenovo_sd650.so \
    ipmi_node_manager.so
endif
ener_DEPS = \
    $(SRCDIR)/metrics/api/energy.h \
    $(SRCDIR)/common/libcommon.a
ener_PATH = $(DESTDIR)/lib/plugins/energy
ener_PERM = 0775

######## RULES

all: $(ener_BINS)

ipmi_ibm.so: ipmi_ibm.c $(ener_DEPS)
	$(CC) $(CFLAGS) -shared -o $@ ipmi_ibm.c $(ener_DEPS)

ipmi_lenovo_sd650.so: ipmi_lenovo_sd650.c $(ener_DEPS)
	$(CC) $(CFLAGS) -shared -o $@ ipmi_lenovo_sd650.c $(ener_DEPS)

ipmi_node_manager.so: ipmi_node_manager.c $(ener_DEPS)
	$(CC) $(CFLAGS) -shared -o $@ ipmi_node_manager.c $(ener_DEPS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ener.ginstall;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
