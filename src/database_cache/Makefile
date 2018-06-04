CFLAGS  = -I$(SRCDIR) $(CONSTANTS)

######## RULES

all: eardbd_api.o client_api_test eardbd

eardbd_api.o:
	$(CC) $(CFLAGS) -c eardbd_api.c

client_api_test:
	$(CC) $(CFLAGS) -o client_api_test tests/client_api_test.c eardbd_api.o

eardbd:
	$(CC) $(CFLAGS) -o eardbd eardbd.c

######## OPTIONS

install:
	mv eardbd $(DESTDIR)/sbin

clean:
	rm -f eardbd client_api_test
	rm -f *.o *.a *.so
