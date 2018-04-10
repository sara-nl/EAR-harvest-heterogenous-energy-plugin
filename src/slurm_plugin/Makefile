CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(CPUF_CFLAGS) $(SLURM_CFLAGS)
LDFLAGS = -shared $(CPUF_LDFLAGS)

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/common/string_enhanced.o

OBJS = slurm_plugin_helper.o

######## RULES

all: earplug.so

slurm_plugin_helper.o: slurm_plugin_helper.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -c slurm_plugin_helper.c

earplug.so: slurm_plugin.c $(OBJS)
	$(CC) $(CFLAGS) -o earplug.so slurm_plugin.c $(LDFLAGS) $(OBJS) $(OUTER_OBJS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
