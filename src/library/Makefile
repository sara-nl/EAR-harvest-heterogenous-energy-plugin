#CFLAGS  = -fPIC -O3 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS) -pthread -DMPI=1
CFLAGS  = -I$(SRCDIR) $(PAPI_CFLAGS) 
LDFLAGS = -shared $(PAPI_LDFLAGS) -ldl

########

ifdef MPI_VERSION
ext = ${MPI_VERSION}.so
else
ext = so
endif

ifdef MPI_CFLAGS
libs_BINS += libear.$(ext)
libs_PATH  = $(DESTDIR)/lib
libs_OBJS  = libear.so
libs_PERM  = 0755
endif

######## FILES

comm_OBJS = \
    $(SRCDIR)/daemon/eard_api.o \
    $(SRCDIR)/daemon/shared_configuration.o \
    $(SRCDIR)/daemon/powercap_status.o \
    $(SRCDIR)/metrics/libmetrics.a \
    $(SRCDIR)/common/libcommon.a

mpix_OBJS = \
    api/ear.o \
    api/ear_mpi.o \
    api/mpic.o \
    api/mpif.o \
		common/global_comm.o

core_OBJS = \
    metrics/metrics.o \
    models/models.o \
    policies/policy.o \
    policies/pc_suport.o \
    states/states.o \
    states/states_periodic.o \
    tracer/tracer.o \
    dynais/dynais.a \
		common/library_shared_data.o 

######## RULES

all: all.components $(libs_OBJS)

%.so: $(mpix_OBJS) $(core_OBJS) $(comm_OBJS)
	$(MPICC) $(MPICC_FLAGS) -o $*.$(ext) $(mpix_OBJS) $(core_OBJS) $(comm_OBJS) $(LDFLAGS)

######## OPTIONS

install: install.components libs.ginstall;

%.install: ;

clean: clean.components rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'api'        $* || exit
	@ $(MAKE) -C 'dynais'     $* || exit
	@ $(MAKE) -C 'loader'     $* || exit
	@ $(MAKE) -C 'metrics'    $* || exit
	@ $(MAKE) -C 'models'     $* || exit
	@ $(MAKE) -C 'policies'   $* || exit
	@ $(MAKE) -C 'states'     $* || exit
	@ $(MAKE) -C 'tracer'     $* || exit
	@ $(MAKE) -C 'common'     $* || exit

######## DEPENDENCIES

depend: depend.components;

depend-clean: depend-clean.components;

include $(SRCDIR)/Makefile.extra
