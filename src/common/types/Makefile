CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES

%.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c  $< 

######## OPTIONS

clean:
	rm -rf *.o *.so *.a
