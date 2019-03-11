CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) -O3 

######## FILES

OBJS = energy_cpu.o 

######## RULES

all: $(OBJS)

energy_cpu.o: energy_cpu.c energy_cpu.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

devel.install: ;
	cp *.h $(DESTDIR)/include/metrics/msr

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
