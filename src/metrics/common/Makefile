CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

OBJS = msr.o

######## RULES

all: all.components $(OBJS)

msr.o: msr.c msr.h
	$(CC) $(CFLAGS) -c msr.c

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend