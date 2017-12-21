DIR          = $(ROOTDIR)/src/slurm_plugin
CFLAGS       = -I$(DIR) -I$(ROOTDIR)/src/common $(CPUF_CFLAGS) $(SLURM_CFLAGS) -fPIC
LDFLAGS      = -shared $(CPUF_LDFLAGS)

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/string_enhanced.o

######## RULES

all: earplug.so

earplug.so: ear_slurm_plugin.c $(OUTER_OBJS)
	gcc $(CFLAGS) -o earplug.so ear_slurm_plugin.c $(LDFLAGS) $(OUTER_OBJS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
