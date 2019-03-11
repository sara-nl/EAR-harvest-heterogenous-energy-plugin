CFLAGS  = -fPIC -I$(SRCDIR) $(CPUF_CFLAGS) $(CONSTANTS)

######## RULES

all: frequency.o

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
