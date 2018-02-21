CFLAGS = -I$(SRCDIR) $(CONSTANTS)

######## RULES

uncores_pci.o: uncores_pci.c uncores_pci.h
	$(CC) $(CFLAGS) -c uncores_pci.c

######## OPTIONS

clean:
	rm -rf *.o *.a *.so
