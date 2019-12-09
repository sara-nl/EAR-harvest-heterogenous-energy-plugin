CFLAGS  = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS)
LDFLAGS = 

######## INSTALLATION

ifdef SLURM_BASE
plug_BINS = earplug.so
else
plug_BINS = erun
endif

plug_PATH = $(DESTDIR)/lib
plug_PERM = 0775

######## FILES

plug_OBJS = \
        $(SRCDIR)/daemon/eard_rapi.o \
        $(SRCDIR)/daemon/shared_configuration.o \
        $(SRCDIR)/global_manager/eargm_rapi.o \
        $(SRCDIR)/common/libcommon.a

plug_SRCS = \
        slurm_plugin.c \
        slurm_plugin_environment.c \
        slurm_plugin_options.c \
        slurm_plugin_rcom_eard.c \
        slurm_plugin_rcom_eargmd.c \
        slurm_plugin_rcom_shared.c \
        slurm_plugin_serialization.c \
        slurm_plugin_test.c

plug_HDRS = \
        slurm_plugin.h \
        slurm_plugin_environment.h \
        slurm_plugin_options.h \
        slurm_plugin_rcom.h \
        slurm_plugin_serialization.h \
        slurm_plugin_test.h

######## RULES

all: $(plug_BINS)

earplug.so: $(plug_SRCS) $(plug_HDRS)
	$(CC) $(CFLAGS) -shared -o $@ $(plug_SRCS) $(plug_OBJS) $(LDFLAGS)

erun: erun.c erun.h $(plug_SRCS) $(plug_HDRS)
	$(CC) $(CFLAGS) -DERUN -g -o $@ $< $(plug_SRCS) $(plug_OBJS)

######## OPTIONS

install: plug.ginstall;

%.install: ;

clean: plug.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
