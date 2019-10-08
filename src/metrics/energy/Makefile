CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -DSHOW_DEBUGS

######## FILES

ener_BINS = energy_cpu.o energy_node.o
ener_DEPS = \
    $(SRCDIR)/common/libcommon.a

######## RULES

all: all.components $(ener_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'cpu' $* || exit

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
