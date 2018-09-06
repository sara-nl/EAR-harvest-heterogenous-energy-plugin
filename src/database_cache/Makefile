CFLAGS  = -I$(SRCDIR) $(CONSTANTS)
LDFLAGS = $(DB_LDFLAGS) -lm

######## FILES

eardbd_OBJS = $(SRCDIR)/common/sockets.o \
              $(SRCDIR)/common/database.a \
              $(SRCDIR)/common/types.a

######## RULES

all: eardbd_api.o eardbd

eardbd_api.o: eardbd_api.c eardbd_api.h
	$(CC) $(CFLAGS) -c eardbd_api.c

eardbd: eardbd.c eardbd.h $(eardbd_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o eardbd eardbd.c $(LDFLAGS) $(eardbd_OBJS)

######## OPTIONS

install:
	cp eardbd $(DESTDIR)/sbin

%.install: ;

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so eardbd

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
