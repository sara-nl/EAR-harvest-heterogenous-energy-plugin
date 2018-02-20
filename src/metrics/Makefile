CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS = $(PAPI_LDFLAGS)

######## FILES

BINARIES = generics.o \
           cache.o    \
           flops.o    \
           intructions.o \
           stalls.o

######## RULES

all: $(BINARIES)

generics.o: generics.c generics.h
	$(CC) $(CFLAGS) -c generics.c $(LDFLAGS)

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c $(LDFLAGS)

flops.o: flops.c flops.h
	$(CC) $(CFLAGS) -c flops.c $(LDFLAGS)

intructions.o: intructions.c intructions.h
	$(CC) $(CFLAGS) -c intructions.c $(LDFLAGS)

stalls.o: stalls.c stalls.h
	$(CC) $(CFLAGS) -c stalls.c $(LDFLAGS)

install:

clean:
	rm -rf $(BINARIES)
	rm -f *.so *.o