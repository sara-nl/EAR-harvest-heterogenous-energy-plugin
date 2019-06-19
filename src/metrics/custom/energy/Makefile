CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

ener_OBJS = finder.o \
    ipmi_lenovo.o

######## RULES

all: $(ener_OBJS)

ipmi_lenovo.o: ipmi_lenovo.c ipmi_lenovo.h
	$(CC) $(CFLAGS) -shared -o ipmi.lenovo.so ipmi_lenovo.c

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
