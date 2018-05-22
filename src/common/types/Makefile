CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES

%.o: %.c %.h $(DEPS)
	$(CC) $(CFLAGS) -c  $< 

######## OPTIONS

clean:
	rm -rf *.o *.so *.a
