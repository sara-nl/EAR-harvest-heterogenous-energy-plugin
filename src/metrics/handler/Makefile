CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) -Wno-unused-variable

######## FILES

find_OBJS = \
    energy.o \
		energy_lib.o

######## RULES

all: $(find_OBJS)

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
