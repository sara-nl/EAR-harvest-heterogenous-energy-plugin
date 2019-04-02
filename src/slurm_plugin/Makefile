CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS) $(SLURM_CFLAGS)
LDFLAGS = 

######## INSTALLATION

plug_BINS = earplug.so
plug_PATH = $(DESTDIR)/lib
plug_PERM = 0666

######## FILES

plug_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
        $(SRCDIR)/daemon/shared_configuration.o \
        $(SRCDIR)/global_manager/eargm_rapi.o \
        $(SRCDIR)/common/libcommon.a

plug_SRCS = slurm_plugin.c \
        slurm_plugin_helper.c \
        slurm_plugin_options.c \
        slurm_plugin_reports.c

plug_HDRS = slurm_plugin.h \
        slurm_plugin_helper.h \
        slurm_plugin_options.h \
        slurm_plugin_reports.c

######## RULES

all: $(plug_BINS)

earplug.so: $(plug_SRCS) $(plug_HDRS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared -o $@ $(plug_SRCS) $(plug_OBJS) $(LDFLAGS)

######## OPTIONS

install: plug.ginstall;

%.install: ;

clean: rclean

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
