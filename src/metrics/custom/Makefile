CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)
LDFLAGS =

######## FILES

BANDWIDTH_OBJS = bandwidth/uncores_pci.o

LIBRARIES = bandwidth.a \
            frequency.o \
            hardware_info.o

######## RULES

all: $(LIBRARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

bandwidth.o: bandwidth.c bandwidth.h
	$(CC) $(CFLAGS) -c bandwidth.c $(LDFLAGS)

bandwidth.a: bandwidth.o $(BANDWIDTH_OBJS)
	ar rvs bandwidth.a bandwidth.o $(BANDWIDTH_OBJS)

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c $(LDFLAGS)

hardware_info.o: hardware_info.c hardware_info.h
	$(CC) $(CFLAGS) -c hardware_info.c $(LDFLAGS)

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so
	$(MAKE) -C bandwidth clean
