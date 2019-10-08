CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

ener_BINS =
ener_DEPS = \
    $(SRCDIR)/metrics/api/energy.h \
    $(SRCDIR)/common/libcommon.a
ener_PATH = $(DESTDIR)/lib/plugins/energy
ener_PERM = 0775

######## RULES

all: $(ener_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ener.ginstall;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
