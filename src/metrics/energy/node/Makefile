CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

ener_OBJS = \
    energy_nm.o \
    energy_rapl.o
ener_BINS = \
    energy_nm.so \
    energy_rapl.so
ener_PATH = $(DESTDIR)/lib/plugins/energy
ener_PERM = 0775

######## RULES

all: $(ener_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.so: %.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 

######## OPTIONS

install: ener.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
