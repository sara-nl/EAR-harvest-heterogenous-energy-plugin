CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

powercap_OBJS = \
    inm.o \
		pc_dvfs.o \
		gpu_dummy.o

powercap_BINS = \
    inm.so \
		pc_dvfs.so \
		gpu_dummy.so

powercap_PATH = $(DESTDIR)/lib/plugins/powercap
powercap_PERM = 0775
dvfs_DEPS = \
	$(SRCDIR)/daemon/powercap_status.o \
	$(SRCDIR)/metrics/libmetrics.a \
	$(SRCDIR)/common/libcommon.a 

######## RULES

all: $(powercap_BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
pc_dvfs.so:pc_dvfs.o $(dvfs_DEPS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 


%.so: %.o 
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 

######## OPTIONS

install: powercap.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
