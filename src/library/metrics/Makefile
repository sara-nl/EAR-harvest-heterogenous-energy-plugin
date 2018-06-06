all: metrics.o

metrics.o: metrics.c metrics.h
	$(CC) $(CFLAGS) -c metrics.c

######## OPTIONS 

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
