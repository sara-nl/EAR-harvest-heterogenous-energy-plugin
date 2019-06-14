######## FILES

OBJS = models.o \
       min_time.o \
       min_energy.o \
       monitoring.o \
       supermucng.c \
       dyn_policies.c

########

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -c $<

######## OPTIONS 

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
