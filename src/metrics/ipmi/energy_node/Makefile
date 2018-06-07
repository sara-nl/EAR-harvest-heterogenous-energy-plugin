CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

OBJS = ibm.o \
       lenovo_nm.o \
       lenovo_ina226.o

######## RULES

all: $(OBJS)

lenovo_nm.o: lenovo_nm.c lenovo_nm.h
	$(CC) $(CFLAGS) -c lenovo_nm.c

lenovo_ina226.o: lenovo_ina226.c lenovo_ina226.h
	$(CC) $(CFLAGS) -c lenovo_ina226.c

ibm.o: ibm.c ibm.h
	$(CC) $(CFLAGS) -c ibm.c

######## OPTIONS

clean:
	rm -rf *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
