CFLAGS  = -Wall -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS)
LDFLAGS = 

######## FILES

OBJECTS = $(SRCDIR)/daemon/eard_rapi.o \
          $(SRCDIR)/daemon/shared_configuration.o \
          $(SRCDIR)/global_manager/eargm_rapi.o \
          $(SRCDIR)/common/types.a

SOURCES = slurm_plugin.c \
          slurm_plugin_helper.c \
          slurm_plugin_options.c \
          slurm_plugin_reports.c

HEADERS = slurm_plugin.h \
          slurm_plugin_helper.h \
          slurm_plugin_options.h \
          slurm_plugin_reports.c

######## RULES

all: earplug.so

earplug.so: $(SOURCES) $(HEADERS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared -o earplug.so $(SOURCES) $(OBJECTS) $(LDFLAGS)

######## OPTIONS

install:
	cp earplug.so $(DESTDIR)/lib

%.install:

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
