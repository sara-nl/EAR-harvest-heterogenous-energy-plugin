CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES

all: uncores_pci.o

uncores_pci.o: uncores_pci.c uncores_pci.h
	$(CC) $(CFLAGS) -c uncores_pci.c

######## OPTIONS

install:

%.install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
