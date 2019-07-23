CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

find_OBJS = \
    energy.o

######## RULES

all: $(find_OBJS)

energy.o: energy.c energy.h
	$(CC) $(CFLAGS) $(IPMI_CFLAGS) -c $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
