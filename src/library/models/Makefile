######## FILES

OBJS = models.o \
       min_time.o \
       min_energy.o \
       monitoring.o \
       sig_projections.o

########

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS 

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
