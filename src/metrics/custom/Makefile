CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)
LDFLAGS =

######## FILES

BANDWIDTH_OBJS = bandwidth/uncores_pci.o

BINARIES = bandwidth.o \
           frequency.o \
           hardware_info.o

######## RULES

all: $(BINARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

bandwidth.o: bandwidth.c bandwidth.h $(BANDWIDTH_OBJS)
	$(CC) $(CFLAGS) -c bandwidth.c $(LDFLAGS)

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c $(LDFLAGS)

hardware_info.o: hardware_info.c hardware_info.h
	$(CC) $(CFLAGS) -c hardware_info.c $(LDFLAGS)

######## OPTIONS

install:

clean:
	rm -rf $(BINARIES)
	rm -f *.so *.o
