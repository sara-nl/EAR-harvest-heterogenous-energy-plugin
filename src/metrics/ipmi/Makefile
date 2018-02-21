CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)
LDFLAGS =

######## FILES

ENERGY_NODE_OBJS = energy_node/ibm.o \
                   energy_node/lenovo_act.o \
                   energy_node/lenovo_wct.o

LIBRARIES = energy_node.a

######## RULES

all: $(LIBRARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

energy_node.o: energy_node.c energy_node.h $(ENERGY_NODE_OBJS)
	$(CC) $(CFLAGS) -c energy_node.c

energy_node.a: energy_node.o $(ENERGY_NODE_OBJS)
	ar rvs energy_node.a energy_node.o $(ENERGY_NODE_OBJS)

######## OPTIONS

install:

clean:
	rm -rf $(BINARIES)
	rm -f *.so *.o
