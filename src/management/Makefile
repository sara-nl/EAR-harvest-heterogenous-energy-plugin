######## FILES

stat_BINS = libmgt.a

mana_OBJS = \
    $(SRCDIR)/management/gpu/gpu.o \
    $(SRCDIR)/management/gpu/archs/dummy.o

ifdef CUDA_BASE
mana_OBJS += \
    $(SRCDIR)/management/gpu/archs/nvml.o
endif

######## RULES

all: all.components $(stat_BINS)

libmgt.a: $(mana_OBJS)
	$(AR) rvs $@ $(mana_OBJS)

######## OPTIONS

install: install.components;

devel.install: ;

%.install: ;

clean: clean.components stat.gclean;

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'gpu'    $* || exit
	@ $(MAKE) -C 'pstate' $* || exit

######## EXPORTS

include $(SRCDIR)/Makefile.extra
