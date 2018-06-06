CFLAGS  = -I$(SRCDIR) $(CONSTANTS)

######## FILES

OBJS = eardbd_api.o

BINARIES = eardbd

######## RULES

all: $(OBJS) $(BINARIES)

eardbd_api.o:
	$(CC) $(CFLAGS) -c eardbd_api.c

eardbd:
	$(CC) $(CFLAGS) -o eardbd eardbd.c

client_api_test: eardbd_api.o
	$(CC) $(CFLAGS) -o client_api_test tests/client_api_test.c eardbd_api.o

######## OPTIONS

install:
	mv eardbd $(DESTDIR)/sbin

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so eardbd

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend