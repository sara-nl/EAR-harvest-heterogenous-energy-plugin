CFLAGS = $(CC_FLAGS) -I$(SRCDIR) $(PAPI_CFLAGS) -O2 -fPIC

########

all: metrics.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
