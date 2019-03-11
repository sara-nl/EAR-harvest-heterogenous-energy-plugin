CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## FILES

cstm_OBJS = frequency.o \
    frequency_uncore.o \
    hardware_info.o \
    bandwidth.o \
    bandwidth_notpriv.o \
    bandwidth

######## RULES

all: all.components $(cstm_OBJS)

frequency.o: frequency.c frequency.h
	$(CC) $(CFLAGS) -c $<

frequency_uncore.o: frequency_uncore.c frequency_uncore.h
	$(CC) $(CFLAGS) -c $<

hardware_info.o: hardware_info.c hardware_info.h
	$(CC) $(CFLAGS) -c $<

bandwidth.o: bandwidth.c bandwidth.h
	$(CC) $(CFLAGS) -c $<

bandwidth_notpriv.o: bandwidth_notpriv.c bandwidth_notpriv.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: install.components

devel.install: ;

%.install: %.install.components;

clean: clean.components rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'bandwidth' $* || exit

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra