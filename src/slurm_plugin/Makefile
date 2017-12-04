DIR          = $(ROOTDIR)/src/slurm_plugin
CFLAGS       = -I$(DIR) -I$(ROOTDIR)/src/common $(CPUF_CFLAGS) $(SLURM_CFLAGS) -fPIC
LDFLAGS      = -shared $(CPUF_LDFLAGS)

######## RULES

all: ear_slurm_plugin.so

ear_slurm_plugin.so: ear_slurm_plugin.c
	gcc $(CFLAGS) -o ear_slurm_plugin.so ear_slurm_plugin.c $(LDFLAGS)

######## OPTIONS

install:
	mv ear_slurm_plugin.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
