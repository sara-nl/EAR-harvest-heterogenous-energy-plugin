CFLAGS  = -I$(SRCDIR) $(CONSTANTS)

######## FILES

#eardbd_OBJS = $(SRCDIR)/common/database.a \
#			   $(SRCDIR)/common/types.a

eardbd_OBJS = $(SRCDIR)/common/types.a \
              sockets.o

OBJS = sockets.o \
       eardbd_api.o

BINARIES = eardbd

######## RULES

all: $(OBJS) $(BINARIES)

sockets.o: sockets.c sockets.h
	$(CC) $(CFLAGS) -c sockets.c

eardbd_api.o: eardbd_api.c eardbd_api.h
	$(CC) $(CFLAGS) -c eardbd_api.c

eardbd: eardbd.c eardbd.h
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o eardbd eardbd.c $(DB_LDFLAGS) $(eardbd_OBJS)

client_api_test: eardbd_api.o
	$(CC) $(CFLAGS) -o client_api_test tests/client_api_test.c eardbd_api.o

######## OPTIONS

install:
	cp eardbd $(DESTDIR)/sbin

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so eardbd

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
