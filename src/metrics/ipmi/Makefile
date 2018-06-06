CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)
LDFLAGS =

######## FILES

ENERGY_OBJS = energy_node/ibm.o \
              energy_node/lenovo_nm.o \
              energy_node/lenovo_ina226.o

OBJS = energy_node.a

######## RULES

all: all.components $(OBJS)

energy_node.o: energy_node.c energy_node.h
	$(CC) $(CFLAGS) -c energy_node.c

energy_node.a: energy_node.o $(ENERGY_OBJS)
	$(AR) rvs energy_node.a energy_node.o $(ENERGY_OBJS)

######## OPTIONS

install: install.components
	cp *.h $(DESTDIR)/include/metrics/ipmi

clean: clean.components
	rm -f *.o *.a *.so

depend: depend.components

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'energy_node' $* || exit
