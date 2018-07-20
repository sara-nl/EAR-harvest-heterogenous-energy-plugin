CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

OBJS = ibm.o \
       lenovo_nm.o \
       lenovo_sd650.o

######## RULES

all: $(OBJS)

lenovo_nm.o: lenovo_nm.c lenovo_nm.h
	$(CC) $(CFLAGS) -c lenovo_nm.c

lenovo_sd650.o: lenovo_sd650.c lenovo_sd650.h
	$(CC) $(CFLAGS) -c lenovo_sd650.c

ibm.o: ibm.c ibm.h
	$(CC) $(CFLAGS) -c ibm.c

######## OPTIONS

install:

clean:
	rm -rf *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
