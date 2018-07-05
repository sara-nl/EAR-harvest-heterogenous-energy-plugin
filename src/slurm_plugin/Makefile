CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS) $(CPUF_CFLAGS) -O0
LDFLAGS = $(CPUF_LDFLAGS)

######## FILES

plugin_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
              $(SRCDIR)/global_manager/eargm_rapi.o \
              $(SRCDIR)/common/types.a

plugin_SOURCES = slurm_plugin.c \
                 slurm_plugin_helper.c \
                 slurm_plugin_options.c \
                 slurm_plugin_user_environment.c

######## RULES

all: earplug.so

earplug.so: $(plugin_SOURCES) slurm_plugin.h slurm_plugin_helper.h
	$(CC) $(CFLAGS) -shared -o earplug.so $(plugin_SOURCES) $(plugin_OBJS) $(LDFLAGS)

######## OPTIONS

install:
	cp earplug.so $(DESTDIR)/lib

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
