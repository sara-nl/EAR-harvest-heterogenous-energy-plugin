CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common $(CONSTANTS)

######## RULES

pci_uncores.o: pci_uncores.c pci_uncores.h
	$(CC) $(CFLAGS) -c pci_uncores.c

######## OPTIONS

clean:
	rm -rf *.o
