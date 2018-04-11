CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(CPUF_CFLAGS) $(SLURM_CFLAGS)
LDFLAGS = -shared $(CPUF_LDFLAGS)
CC = gcc

######## FILES

OBJS = slurm_plugin_helper.o

######## RULES

all: earplug.so

slurm_plugin_helper.o: slurm_plugin_helper.c
	$(CC) -Wall $(CFLAGS) -c slurm_plugin_helper.c

earplug.so: slurm_plugin.c $(OBJS)
	$(CC) -Wall $(CFLAGS) -o earplug.so slurm_plugin.c $(LDFLAGS) $(OBJS)

######## OPTIONS

install:
	mv earplug.so $(DESTDIR)/lib

clean:
	rm -f *.so *.o
