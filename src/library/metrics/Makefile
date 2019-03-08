all: metrics.o

metrics.o: metrics.c metrics.h
	$(CC) $(CFLAGS) -c metrics.c

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
