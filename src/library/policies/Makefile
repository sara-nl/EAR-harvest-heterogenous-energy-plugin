######## FILES

modl_OBJS = min_time.o \
       min_energy.o \
       monitoring.o 

modl_BINS = min_time.so min_energy.so monitoring.so
modl_PATH = $(DESTDIR)/lib/plugins
modl_PERM = 0775

########

all: $(modl_OBJS) $(modl_BINS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -c $<

%.so: %.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 	

######## OPTIONS 

install: modl.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
