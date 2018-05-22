CFLAGS  = -g -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = -shared $(CPUF_LDFLAGS)
CC = gcc

######## FILES

OBJS = slurm_plugin_helper.o

HEADERS = $(SRCDIR)/common/config.h

######## RULES

all: earplug.so

slurm_plugin_helper.o: slurm_plugin_helper.h slurm_plugin_helper.c $(HEADERS)
	$(CC) $(CFLAGS) -c slurm_plugin_helper.c

earplug.so: slurm_plugin.h slurm_plugin.c $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o earplug.so slurm_plugin.c $(LDFLAGS) $(OBJS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
