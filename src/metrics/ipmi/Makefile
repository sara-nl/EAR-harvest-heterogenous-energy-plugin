CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)
LDFLAGS =

######## FILES

ENERGY_NODE_OBJS = energy_node/ibm.o \
                   energy_node/lenovo_nm.o \
                   energy_node/lenovo_ina226.o

LIBRARIES = energy_node.a energy_node.so

######## RULES

all: $(LIBRARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

energy_node.o: energy_node.c energy_node.h $(ENERGY_NODE_OBJS)
	$(CC) $(CFLAGS) -c energy_node.c

energy_node.so: energy_node.o $(ENERGY_NODE_OBJS)
	$(CC) -shared -o energy_node.so energy_node.o $(ENERGY_NODE_OBJS)

energy_node.a: energy_node.o $(ENERGY_NODE_OBJS)
	ar rvs energy_node.a energy_node.o $(ENERGY_NODE_OBJS)

######## OPTIONS

install:
	cp *.h $(DESTDIR)/include/metrics/ipmi

clean:
	rm -f $(BINARIES)
	rm -f *.so *.a *.o *.so
	@ $(MAKE) -C energy_node clean
