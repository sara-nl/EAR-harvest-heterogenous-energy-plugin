CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS) -O2

######## INSTALLATION

libc_BINS = libcommon.a
libc_OBJS = $(patsubst %.c, %.o, $(shell find -maxdepth 1 -type f -name '*.c'))
incl_BINS = states.h
incl_PATH = $(DESTDIR)/include
incl_PERM = 0660

######## FILES

######## RULES

all: all.components $(libc_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

libcommon.a: $(libc_OBJS)
	$(AR) rvs $@ $(shell find ./ -type f -name '*.o')

######## OPTIONS

install: install.components;

devel.install: devel.install.components incl.ginstall;

%.install: %.install.components;

clean: clean.components libc.gclean rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'types' $* || exit
ifdef DB_BASE
	@ $(MAKE) -C 'database' $* || exit
endif

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
