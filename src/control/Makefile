CFLAGS  = -I$(SRCDIR) $(CONSTANTS) 

######## VARS

######## RULES

all: frequency.o

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c

######## OPTIONS

install:

clean:
	rm -f *.o *.a *.so
