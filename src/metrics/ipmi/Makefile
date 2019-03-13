CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)
LDFLAGS =

######## FILES

lib_OBJS = energy_node/ibm.o \
           energy_node/lenovo_nm.o \
           energy_node/lenovo_sd650.o

OBJS = energy_node.a

######## RULES

all: all.components $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

energy_node.a: energy_node.o $(lib_OBJS)
	$(AR) rvs $@ $< $(lib_OBJS)

######## OPTIONS

install: install.components

devel.install: devel.install.components;

%.install: %.install.components;

clean: clean.components rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'energy_node' $* || exit

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
