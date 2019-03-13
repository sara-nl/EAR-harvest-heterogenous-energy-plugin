all: dynais.o

dynais.o: dynais.c dynais.h
	$(CC) $(CFLAGS) -march=native -c dynais.c

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra