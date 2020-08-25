CFLAGS  = -fPIC -O2 -I$(SRCDIR) -DMPI=$(FEAT_MPI_API)
LDFLAGS = -shared -ldl -lm -lpthread

ifeq ($(FEAT_MPI_API), 0)
MPICC       = $(CC)
MPICC_FLAGS = $(CC_FLAGS)
MPI_VERSION = seq
endif

export MPICC
export MPICC_FLAGS
export CFLAGS

########

ifdef MPI_VERSION
ext = ${MPI_VERSION}.so
else
ext = so
endif

libs_BINS  = libear.$(ext)
libs_PATH  = $(DESTDIR)/lib
libs_OBJS  = libear.so
libs_PERM  = 0755

######## FILES

comm_OBJS = \
    $(SRCDIR)/daemon/local_api/eard_api.o \
    $(SRCDIR)/daemon/shared_configuration.o \
    $(SRCDIR)/daemon/powercap/powercap_status.o \
    $(SRCDIR)/metrics/libmetrics.a \
    $(SRCDIR)/management/libmgt.a \
    $(SRCDIR)/common/libcommon.a

core_OBJS = \
    api/ear.o \
    common/library_shared_data.o \
    metrics/metrics.o \
    metrics/gpu.o \
    models/models.o \
    policies/policy.o \
    policies/pc_suport.o \
    states/states.o \
    states/states_periodic.o \
    tracer/tracer.o \
    dynais/dynais.a

ifeq ($(FEAT_MPI_API), 1)
mpix_OBJS = \
    api/ear_mpi.o \
    api/ear_default.o \
    api/mpic.o \
    api/mpif.o \
    common/global_comm.o
endif

######## RULES

all: all.components $(libs_OBJS)

%.so: $(mpix_OBJS) $(core_OBJS) $(comm_OBJS)
	$(MPICC) $(MPICC_FLAGS) -o $*.$(ext) $^ $(LDFLAGS)

######## OPTIONS

install: install.components libs.ginstall;

%.install: ;

clean: clean.components rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'api'        $* || exit
	@ $(MAKE) -C 'common'     $* || exit
	@ $(MAKE) -C 'dynais'     $* || exit
	@ $(MAKE) -C 'loader'     $* || exit
	@ $(MAKE) -C 'metrics'    $* || exit
	@ $(MAKE) -C 'models'     $* || exit
	@ $(MAKE) -C 'policies'   $* || exit
	@ $(MAKE) -C 'states'     $* || exit
	@ $(MAKE) -C 'tracer'     $* || exit

######## DEPENDENCIES

depend: depend.components;

depend-clean: depend-clean.components;

include $(SRCDIR)/Makefile.extra
