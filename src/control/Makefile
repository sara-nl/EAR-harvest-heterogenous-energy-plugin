CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES

all: frequency.o cpupower.o

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c frequency.c

cpupower.o:cpupower.c cpupower.h
	$(CC) $(CFLAGS) -c cpupower.c

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
