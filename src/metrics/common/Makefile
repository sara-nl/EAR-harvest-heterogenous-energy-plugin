CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) 

######## FILES

comm_OBJS = msr.o cpuid.o

######## RULES

all: $(comm_OBJS)

msr.o: msr.c msr.h
	$(CC) $(CFLAGS) -c $<

cpuid.o: cpuid.c cpuid.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
