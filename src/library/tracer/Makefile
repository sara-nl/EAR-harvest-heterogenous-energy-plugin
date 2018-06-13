######## RULES

all: tracer.o

allt: DEFINES=-DEAR_GUI
allt: tracer.o

%.o: %.c %.h
	$(CC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
