CFLAGS  = -fPIC -I$(SRCDIR) $(CPUF_CFLAGS) $(CONSTANTS)

######## VARS

HEADERS = $(SRCDIR)/common/ear_verbose.h \
          $(SRCDIR)/common/states.h

######## RULES

all: frequency.o

frequency.o: frequency.c frequency.h $(HEADERS)
	$(CC) $(CFLAGS) -c frequency.c

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend