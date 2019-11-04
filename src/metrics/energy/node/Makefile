CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

ener_OBJS = \
    energy_nm.o \
    energy_rapl.o
ener_BINS = \
    energy_nm.so \
    energy_rapl.so

rapl_DEPS = $(SRCDIR)/metrics/energy/energy_cpu.o $(SRCDIR)/metrics/common/msr.o $(SRCDIR)/common/hardware/hardware_info.o \
$(SRCDIR)/common/hardware/cpuid.o 
ener_PATH = $(DESTDIR)/lib/plugins/energy
ener_PERM = 0775
ener_DEPS = $(SRCDIR)/common/libcommon.a

######## RULES

all: $(ener_BINS)

%.o: %.c
	$(CC) $(CFLAGS)  -c $<

energy_rapl.so:energy_rapl.o $(rapl_DEPS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ $(ener_DEPS) $(rapl_DEPS)

%.so: %.o $(ener_DEPS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ $(ener_DEPS) 

######## OPTIONS

install: ener.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
