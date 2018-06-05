CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = -shared $(CPUF_LDFLAGS)

######## FILES

HEADERS = $(SRCDIR)/common/config.h

OBJS = slurm_plugin_helper.o \
       earplug.so

######## RULES

all: $(OBJS)

slurm_plugin_helper.o: slurm_plugin_helper.h slurm_plugin_helper.c $(HEADERS)
	$(CC) $(CFLAGS) -c

earplug.so: slurm_plugin.h slurm_plugin.c $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o earplug.so slurm_plugin.c $(LDFLAGS) $(OBJS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.o *.a *.so
