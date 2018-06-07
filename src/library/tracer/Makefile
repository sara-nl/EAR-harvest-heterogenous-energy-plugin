######## RULES

all: tracer.o

allt: $(eval DEFINES=-DEAR_GUI) tracer.o

%.o: %.c %.h
	$(CC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
