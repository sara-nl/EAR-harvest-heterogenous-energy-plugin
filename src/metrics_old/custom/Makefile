CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) -Wno-unused-variable

######## FILES

cstm_OBJS = \
    frequency.o \
    frequency_uncore.o \
    hardware_info.o \
    bandwidth.o \
    bandwidth_notpriv.o \

######## RULES

all: all.components $(cstm_OBJS)

%.o: %.c %.h
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
