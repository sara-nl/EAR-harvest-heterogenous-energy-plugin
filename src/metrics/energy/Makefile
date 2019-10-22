CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -DSHOW_DEFINES

######## FILES

ener_BINS = \
    energy_cpu.o \
    energy_node.o \
    energy_node_lib.o
ener_DEPS = \
    $(SRCDIR)/common/libcommon.a

######## RULES

all: all.components $(ener_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'node' $* || exit

######## OPTIONS

install: install.components;

%.install: %.install.components;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
