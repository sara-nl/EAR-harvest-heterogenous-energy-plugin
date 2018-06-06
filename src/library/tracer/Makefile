######## RULES

all: tracer.o

allt: $(eval DEFINES=-DEAR_GUI) tracer.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
