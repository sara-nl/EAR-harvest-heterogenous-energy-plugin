######## FILES

stat_BINS = libmgt.a

mana_OBJS = \
    $(SRCDIR)/management/gpu/gpu.o

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

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'gpu' $* || exit

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

######## DEPENDENCIES

include $(SRCDIR)/Makefile.extra
