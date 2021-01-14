######## FILES

modl_BINS = \
    basic_model.so \
    avx512_model.min_time.so \
    avx512_model.min_energy.so 
modl_OBJS = models.o
modl_PATH = $(DESTDIR)/lib/plugins/models
modl_PERM = 0775

########

all: $(modl_OBJS) $(modl_BINS)

%.o: %.c %.h
	$(CC) $(CC_FLAGS) $(LIB_CFLAGS) $(CFLAGS) -c $<

%.so: %.o
	$(CC) $(CC_FLAGS) -shared -o $@ $^ 

######## OPTIONS 

install: modl.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
