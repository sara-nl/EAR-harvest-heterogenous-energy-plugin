CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

ener_OBJS = finder.o \
    ipmi_node_manager.so
#    ipmi_lenovo_sd650.so \
#    ipmi_ibm.so \

######## RULES

all: $(ener_OBJS)


ipmi_ibm.so: ipmi_ibm.c ipmi_ibm.h
	$(CC) $(CFLAGS) -shared -o ipmi.ibm.so ipmi_ibm.c

ipmi_lenovo_sd650.so: ipmi_lenovo_sd650.c ipmi_lenovo_sd650.h
	$(CC) $(CFLAGS) -shared -o ipmi.lenovo.sd650.so ipmi_lenovo_sd650.c

ipmi_node_manager.so: ipmi_node_manager.c ipmi_node_manager.h
	$(CC) $(CFLAGS) -shared -o ipmi.node.manager.so ipmi_node_manager.c

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
