CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = $(CPUF_LDFLAGS)

######## RULES

all: earplug.so

earplug.so: slurm_plugin.h slurm_plugin.c slurm_plugin_helper.c slurm_plugin_helper.h
	$(CC) $(CFLAGS) -shared -o earplug.so slurm_plugin.c slurm_plugin_helper.c $(LDFLAGS)

######## OPTIONS

install:
	cp earplug.so $(DESTDIR)/lib

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
