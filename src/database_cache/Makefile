CFLAGS  = -I$(SRCDIR) $(CONSTANTS)
LDFLAGS = $(DB_LDFLAGS) -lm

######## FILES

eardbd_OBJS = $(SRCDIR)/common/sockets.o \
              $(SRCDIR)/common/types.a
#	              $(SRCDIR)/common/database.a \

eardbd_SRCS = eardbd.c \
              eardbd_body.c \
              eardbd_sync.c \
              eardbd_signals.c \
              eardbd_storage.c

eardbd_HDRS = eardbd.h \
              eardbd_body.h \
              eardbd_sync.h \
              eardbd_signals.h \
              eardbd_storage.h

######## RULES

all: eardbd_api.o eardbd

eardbd_api.o: eardbd_api.c eardbd_api.h
	$(CC) $(CFLAGS) -c $<

eardbd: $(eardbd_SRCS) $(eardbd_HDRS) $(eardbd_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $(eardbd_SRCS) $(LDFLAGS) $(eardbd_OBJS)

######## OPTIONS

install:
	cp eardbd $(DESTDIR)/sbin

%.install: ;

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so eardbd

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
