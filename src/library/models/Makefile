######## FILES

modl_OBJS = models.o \
       min_time.o \
       min_energy.o \
       monitoring.o \
       basic_model.o \

modl_BINS = libbasic_model.so
modl_PATH = $(DESTDIR)/lib/plugins
modl_PERM = 0775

########

all: $(modl_OBJS) $(modl_BINS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -c $<

libbasic_model.so: basic_model.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared -o libbasic_model.so basic_model.o	

######## OPTIONS 

install: modl.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
