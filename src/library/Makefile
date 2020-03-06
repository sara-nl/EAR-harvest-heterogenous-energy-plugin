CFLAGS  = -I$(SRCDIR) $(PAPI_CFLAGS) 
LDFLAGS = -shared $(PAPI_LDFLAGS) -ldl

######## VARS

export DIR
export CFLAGS
export LDFLAGS

########

ifdef MPI_VERSION
ext = ${MPI_VERSION}.so
else
ext = so
endif

ifdef MPICC
libs_BINS += libear.$(ext)
libs_PATH  = $(DESTDIR)/lib
libs_OBJS  = libear.so
libs_PERM  = 0755
endif

######## FILES

comm_OBJS = \
    $(SRCDIR)/daemon/eard_api.o \
    $(SRCDIR)/daemon/shared_configuration.o \
    $(SRCDIR)/metrics/libmetrics.a \
    $(SRCDIR)/common/libcommon.a

mpix_OBJS = \
    api/api.o \
    api/api_ear.o \
    api/earc_syms.o \
    api/earf_syms.o \
    metrics/metrics.o \
    models/models.o \
    policies/policy.o \
    states/states.o \
    states/states_periodic.o \
    tracer/tracer.o \
    dynais/dynais.a

######## RULES

all: all.components $(libs_OBJS)

%.so: $(mpix_OBJS) $(comm_OBJS)
	$(MPICC) $(MPICC_FLAGS) -o $*.$(ext) $(mpix_OBJS) $(comm_OBJS) $(LDFLAGS)

######## OPTIONS

install: install.components libs.ginstall;

%.install: ;

clean: clean.components rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'api'        $* || exit
	@ $(MAKE) -C 'api_loader' $* || exit
	@ $(MAKE) -C 'dynais'     $* || exit
	@ $(MAKE) -C 'metrics'    $* || exit
	@ $(MAKE) -C 'models'     $* || exit
	@ $(MAKE) -C 'policies'   $* || exit
	@ $(MAKE) -C 'policies'   $* || exit
	@ $(MAKE) -C 'states'     $* || exit
	@ $(MAKE) -C 'tracer'     $* || exit

######## DEPENDENCIES

depend: depend.components;

depend-clean: depend-clean.components;

include $(SRCDIR)/Makefile.extra
