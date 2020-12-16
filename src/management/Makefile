######## FILES

stat_BINS = libmgt.a

mana_OBJS = \
    $(SRCDIR)/management/cpufreq/cpufreq.o \
    $(SRCDIR)/management/cpufreq/connector.o \
    $(SRCDIR)/management/cpufreq/archs/amd17.o \
    $(SRCDIR)/management/cpufreq/archs/dummy.o \
    $(SRCDIR)/management/cpufreq/drivers/acpi_cpufreq.o \
    $(SRCDIR)/management/gpu/gpu.o \
    $(SRCDIR)/management/gpu/archs/dummy.o
    
user_OBJS = \
    $(SRCDIR)/management/cpufreq/cpufreq_user.o \

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
	@ $(MAKE) -C 'gpu'     $* || exit
	@ $(MAKE) -C 'cpufreq' $* || exit

######## EXPORTS

include $(SRCDIR)/Makefile.extra
