DIR          = $(ROOTDIR)/src/slurm_plugin
CFLAGS       = -I$(DIR) -I$(ROOTDIR)/src/common $(CPUF_CFLAGS) $(SLURM_CFLAGS) -fPIC
LDFLAGS      = -shared $(CPUF_LDFLAGS)

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/string_enhanced.o

OBJS = slurm_plugin_helper.o

######## RULES

all: earplug.so

%.o: %.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -c slurm_plugin_helper.c

earplug.so: ear_slurm_plugin.c $(OBJS)
	$(CC) $(CFLAGS) -o earplug.so slurm_plugin.c $(LDFLAGS) $(OBJS) $(OUTER_OBJS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
