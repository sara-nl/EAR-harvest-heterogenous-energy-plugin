CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

ener_BINS = ipmi.node.manager.so
ener_DEPS = \
    $(SRCDIR)/metrics/api/energy.h \
    $(SRCDIR)/common/libcommon.a
ener_PATH = $(DESTDIR)/sbin/plugins
ener_PERM = 0777

######## RULES

all: $(ener_BINS)

ipmi_ibm.so: ipmi_ibm.c
	$(CC) $(CFLAGS) -shared -o ipmi.ibm.so ipmi_ibm.c

ipmi_lenovo_sd650.so: ipmi_lenovo_sd650.c
	$(CC) $(CFLAGS) -shared -o ipmi.lenovo.sd650.so ipmi_lenovo_sd650.c

ipmi.node.manager.so: ipmi_node_manager.c $(ener_DEPS)
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
