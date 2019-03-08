CFLAGS  = -fPIC -I$(SRCDIR) $(CPUF_CFLAGS) $(CONSTANTS)

######## RULES

all: frequency.o

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c

######## OPTIONS

install: ;

%.install: ;

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
