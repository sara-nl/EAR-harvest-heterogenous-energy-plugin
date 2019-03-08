CFLAGS  = -I$(SRCDIR) $(CONSTANTS)
LDFLAGS = $(DB_LDFLAGS) -lm

######## INSTALLATION

eard_BINS = eardbd
eard_PATH = $(DESTDIR)/sbin
eard_PERM = 0750
oths_BINS = eardbd_api.o

######## FILES

eard_OBJS = $(SRCDIR)/common/sockets.o \
    $(SRCDIR)/common/database.a \
    $(SRCDIR)/common/types.a

eard_SRCS = eardbd.c \
    eardbd_body.c \
    eardbd_sync.c \
    eardbd_signals.c \
    eardbd_storage.c

eard_HDRS = eardbd.h \
    eardbd_body.h \
    eardbd_sync.h \
    eardbd_signals.h \
    eardbd_storage.h

######## RULES

all: $(oths_BINS) $(eard_BINS)

eardbd_api.o: eardbd_api.c eardbd_api.h
	$(CC) $(CFLAGS) -c $<

eardbd: $(eard_SRCS) $(eard_HDRS) $(eard_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DB_CFLAGS) -o $@ $(eard_SRCS) $(eard_OBJS) $(LDFLAGS)

######## OPTIONS

install: eard.ginstall;

%.install: ;

clean: eard.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
