######## FILES

OBJS = states.o \
       states_periodic.o

######## RULES

all: $(OBJS)

allt: $(eval DEFINES=-DEAR_GUI) $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(DEFINES) -c $<

######## OPTIONS 

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

