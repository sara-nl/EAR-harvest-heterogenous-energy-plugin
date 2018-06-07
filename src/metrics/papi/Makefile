CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)

######## FILES

OBJS = flops.o \
       cache.o \
       generics.o \
       energy_cpu.o \
       instructions.o \
       stalls.o

######## RULES

all: $(OBJS)

generics.o: generics.c generics.h
	$(CC) $(CFLAGS) -c generics.c

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c

flops.o: flops.c flops.h
	$(CC) $(CFLAGS) -c flops.c

instructions.o: instructions.c instructions.h
	$(CC) $(CFLAGS) -c instructions.c

stalls.o: stalls.c stalls.h
	$(CC) $(CFLAGS) -c stalls.c

energy_cpu.o: energy_cpu.c energy_cpu.h
	$(CC) $(CFLAGS) -c energy_cpu.c

######## OPTIONS

install:
	cp *.h $(DESTDIR)/include/metrics/papi

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
