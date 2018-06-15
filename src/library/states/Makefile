######## FILES

OBJS = states.o \
       states_periodic.o

######## RULES

all: $(OBJS)

allt: DEFINES=-DEAR_GUI
allt: $(OBJS) 

%.o: %.c %.h
	$(MPICC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS 

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

