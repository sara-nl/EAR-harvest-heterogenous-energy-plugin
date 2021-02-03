######## FILES

root_BINS = libmgt.a

root_OBJS = \
    $(SRCDIR)/management/cpufreq/cpufreq.o \
    $(SRCDIR)/management/cpufreq/governor.o \
    $(SRCDIR)/management/cpufreq/frequency.o \
    $(SRCDIR)/management/cpufreq/archs/amd17.o \
    $(SRCDIR)/management/cpufreq/archs/default.o \
    $(SRCDIR)/management/cpufreq/drivers/linux_cpufreq.o \
    $(SRCDIR)/management/gpu/gpu.o \
    $(SRCDIR)/management/gpu/archs/dummy.o
    
ifdef CUDA_BASE
root_OBJS += \
    $(SRCDIR)/management/gpu/archs/nvml.o
endif

######## RULES

all: all.components $(root_BINS)

libmgt.a: Makefile $(root_OBJS)
	$(AR) rvs $@ $(root_OBJS)

######## OPTIONS

install: install.components;

devel.install: ;

%.install: ;

clean: clean.components stat.gclean;

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'gpu'     $* || exit
	@ $(MAKE) -C 'cpufreq' $* || exit

######## EXPORTS

include $(SRCDIR)/Makefile.extra
