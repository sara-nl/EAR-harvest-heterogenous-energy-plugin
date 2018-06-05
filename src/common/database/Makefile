CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS) $(DB_CFLAGS)

######## FILES

HEADERS = $(SRCDIR)/common/config.h

ifdef DB_BASE
OBJS = db_helper.o \
       mysql_io_functions.o
endif

######## RULES

all: $(OBJS)

%.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c  $<

######## OPTIONS

clean:
	rm -f *.o *.so *.a