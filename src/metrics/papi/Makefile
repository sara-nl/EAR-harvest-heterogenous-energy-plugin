CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS) -O2

######## FILES

papi_OBJS = flops.o \
       cache.o \
       generics.o \
       energy_cpu.o \
       instructions.o \
       stalls.o

######## RULES

all: $(papi_OBJS)

generics.o: generics.c generics.h
	$(CC) $(CFLAGS) -c $<

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c $<

flops.o: flops.c flops.h
	$(CC) $(CFLAGS) -c $<

instructions.o: instructions.c instructions.h
	$(CC) $(CFLAGS) -c $<

stalls.o: stalls.c stalls.h
	$(CC) $(CFLAGS) -c $<

energy_cpu.o: energy_cpu.c energy_cpu.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

devel.install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
