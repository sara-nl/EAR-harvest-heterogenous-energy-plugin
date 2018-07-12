CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS) $(DB_CFLAGS)

######## FILES

ifdef DB_BASE
OBJS = db_helper.o \
       mysql_io_functions.o
endif

######## RULES

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

clean:
	rm -f *.o *.so *.a

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
