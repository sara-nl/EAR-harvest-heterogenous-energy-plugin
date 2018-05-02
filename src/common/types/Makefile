CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES
.c.o :
	$(CC) $(CFLAGS) -c -o $@ $< 

######## OPTIONS

clean:
	rm -rf *.o *.so *.a
