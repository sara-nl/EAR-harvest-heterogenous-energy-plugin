CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

HEADERS = $(SRCDIR)/common/config.h 

######## RULES

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) -c  $< 

######## OPTIONS

clean:
	rm -rf *.o *.so *.a
