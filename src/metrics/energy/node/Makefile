CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

ener_OBJS = \
    energy_nm.o \
    energy_nm_backup.o \
    energy_rapl.o
ener_BINS = \
    energy_nm.so \
    energy_nm_backup.so \
    energy_rapl.so
ener_PATH = $(DESTDIR)/lib/plugins/energy
ener_PERM = 0775
ener_DEPS = $(SRCDIR)/common/libcommon.a

######## RULES

all: $(ener_BINS)

%.o: %.c
	$(CC) $(CFLAGS) -DSHOW_DEBUGS -c $<

%.so: %.o $(ener_DEPS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ $(ener_DEPS) 

######## OPTIONS

install: ener.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
