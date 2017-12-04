DIR          = $(ROOTDIR)/src/slurm_plugin
CFLAGS       = -I$(DIR) -I$(ROOTDIR)/src/common $(CPUF_CFLAGS) $(SLURM_CFLAGS) -fPIC
LDFLAGS      = -shared $(CPUF_LDFLAGS)

######## RULES

all: earplug.so

earplug.so: ear_slurm_plugin.c
	gcc $(CFLAGS) -o earplug.so ear_slurm_plugin.c $(LDFLAGS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
