CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

OBJS = uncores_pci.o

######## RULES

all: $(OBJS)

uncores_pci.o: uncores_pci.c uncores_pci.h
	$(CC) $(CFLAGS) -c uncores_pci.c

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so