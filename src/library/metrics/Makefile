########

OBJS = \
    metrics.o \
    gpu.o

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(LIB_CFLAGS) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

depend: ;

depend-clean: ;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
