######## FILES

OBJS = models.o \
       min_time.o \
       min_energy.o \
       monitoring.o 

SO_OBJS=libbasic_model.so

########

all: $(OBJS) $(SO_OBJS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -c $<

libbasic_model.so:basic_model.c
	$(CC) -fPIC -O3 -I$(SRCDIR) $(CC_FLAGS) $(CFLAGS) -olibbasic_model.so -c basic_model.c

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
