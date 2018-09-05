CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

INTERN_OBJS = bandwidth/uncores_pci.o

OBJS = frequency.o \
       frequency_uncore.o \
       hardware_info.o \
       bandwidth.o \
       bandwidth.a

######## RULES

all: all.components $(OBJS)

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c

frequency_uncore.o: frequency_uncore.c frequency_uncore.h
	$(CC) $(CFLAGS) -c frequency_uncore.c

hardware_info.o: hardware_info.c hardware_info.h
	$(CC) $(CFLAGS) -c hardware_info.c

bandwidth.o: bandwidth.c bandwidth.h
	$(CC) $(CFLAGS) -c bandwidth.c

bandwidth.a: bandwidth.o $(INTERN_OBJS)
	$(AR) rcs bandwidth.a bandwidth.o $(INTERN_OBJS)

######## OPTIONS

install: install.components

devel.install: devel.install.components
	cp *.h $(DESTDIR)/include/metrics/custom

clean: clean.components
	rm -f *.o *.a *.so

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'bandwidth' $* || exit
