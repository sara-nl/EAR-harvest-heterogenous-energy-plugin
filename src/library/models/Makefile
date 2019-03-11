######## FILES

OBJS = models.o \
       min_time.o \
       min_energy.o \
       monitoring.o 

########

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra