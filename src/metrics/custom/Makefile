CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)
LDFLAGS =

######## FILES

BANDWIDTH_OBJS = bandwidth/uncores_pci.o

LIBRARIES = frequency.o \
            hardware_info.o \
			bandwidth.so
			bandwidth.a

######## RULES

all: $(LIBRARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

bandwidth.o: bandwidth.c bandwidth.h
	$(CC) $(CFLAGS) -c bandwidth.c $(LDFLAGS)

bandwidth.a: bandwidth.o $(BANDWIDTH_OBJS)
	ar rcs bandwidth.a bandwidth.o $(BANDWIDTH_OBJS) 

bandwidth.so: bandwidth.a $(BANDWIDTH_OBJS)
	$(CC) -shared -o bandwidth.so bandwidth.a

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c $(LDFLAGS)

hardware_info.o: hardware_info.c hardware_info.h
	$(CC) $(CFLAGS) -c hardware_info.c $(LDFLAGS)

######## OPTIONS

install:
	cp *.h $(DESTDIR)/include/metrics/custom

clean:
	rm -f *.o  *.so
	rm $(LIBRARIES) $(BANDWIDTH_OBJS)
