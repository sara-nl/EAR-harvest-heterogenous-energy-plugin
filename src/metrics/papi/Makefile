CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS = $(PAPI_LDFLAGS)

######## FILES

BINARIES = generics.o \
           cache.o    \
           energy_cpu.o   \
           flops.o    \
           instructions.o \
           stalls.o

######## RULES

all: $(BINARIES)

generics.o: generics.c generics.h
	$(CC) $(CFLAGS) -c generics.c $(LDFLAGS)

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c $(LDFLAGS)

flops.o: flops.c flops.h
	$(CC) $(CFLAGS) -c flops.c $(LDFLAGS)

instructions.o: instructions.c instructions.h
	$(CC) $(CFLAGS) -c instructions.c $(LDFLAGS)

stalls.o: stalls.c stalls.h
	$(CC) $(CFLAGS) -c stalls.c $(LDFLAGS)

energy_cpu.o: energy_cpu.c energy_cpu.h
	$(CC) $(CFLAGS) -c energy_cpu.c $(LDFLAGS)

install:

clean:
	rm -rf $(BINARIES)
	rm -f *.so *.o
