CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

powercap_OBJS = \
    inm.o \
		pc_dvfs.o
powercap_BINS = \
    inm.so \
		pc_dvfs.so
powercap_PATH = $(DESTDIR)/lib/plugins/powercap
powercap_PERM = 0775
powercap_DEPS = \
	$(SRCDIR)/daemon/powercap_status.o

######## RULES

all: $(powercap_BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<


%.so: %.o 
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 

######## OPTIONS

install: powercap.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
