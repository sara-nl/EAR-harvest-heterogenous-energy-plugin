CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS) 

######## FILES

papi_OBJS = flops.o \
       cache.o \
       generics.o \
       energy_cpu.o \
       instructions.o \
       stalls.o

######## RULES

all: $(papi_OBJS) basic_test

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

basic_test: basic_test.c
	$(CC) $(CFLAGS) -o $@ $< $(papi_OBJS) $(PAPI_LDFLAGS) ../libmetrics.a

######## OPTIONS

install:

devel.install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
