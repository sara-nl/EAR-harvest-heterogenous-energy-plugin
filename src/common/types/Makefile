CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES
.c.o :
	$(CC) $(CFLAGS) -c  $< 

######## OPTIONS

clean:
	rm -rf *.o *.so *.a
