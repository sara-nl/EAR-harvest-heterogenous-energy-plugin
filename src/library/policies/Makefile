PCFLAGS =  $(CFLAGS) $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) -O2 -fPIC

######## FILES

poli_OBJS = \
    policy.o \
		pc_suport.o


modl_BINS = \
       min_time.so  \
       min_energy.so \
       min_time_no_models.so \
       min_energy_no_models.so \
       monitoring.so 

ifdef MPI_API
modl_BINS += \
			 min_time_mpi_info.so \
			 monitoring_ext.so \
			 monitoring_ext_notime.so 
endif
ifdef CUDA_BASE
modl_BINS += \
			gpu_monitoring.so
endif

modl_PATH = $(DESTDIR)/lib/plugins/policies
modl_PERM = 0775

eard_modl_BINS = \
			min_time_eard.so \
			min_energy_eard.so \
			monitoring_eard.so 

eard_modl_PATH = $(DESTDIR)/lib/plugins/eard_policies
eard_modl_PERM = 0775

deps= $(SRCDIR)/common/libcommon.a $(SRCDIR)/daemon/powercap/powercap_status.o

########

all: $(poli_OBJS) $(modl_BINS) $(eard_modl_BINS)

min_time_eard_eard.o: min_time_eard.c
	$(CC) $(CC_FLAGS)  $(PCFLAGS) -c $<

min_energy_eard.o:min_energy_eard.c
	$(CC) $(CC_FLAGS)  $(PCFLAGS) -c $<
monitoring_eard.o:monitoring_eard.c
	$(CC) $(CC_FLAGS)  $(PCFLAGS) -c $<

%.o: %.c 
	$(CC) $(PCFLAGS) -c $<

%.so: %.o $(deps)
	$(CC) -shared -o $@ $< 

min_energy_eard.so:min_energy_eard.o $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(PCFLAGS) -shared $< -o $@   $(SRCDIR)/common/libcommon.a
min_time_eard.so:min_time_eard.o $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(PCFLAGS) -shared $< -o $@   $(SRCDIR)/common/libcommon.a
monitoring_eard.so:monitoring_eard.o $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(PCFLAGS) -shared $< -o $@   $(SRCDIR)/common/libcommon.a


min_time_mpi_info.so: min_time_mpi_info.o $(SRCDIR)/common/system/time.o
	$(CC) $(CC_FLAGS) $(PCFLAGS) -shared $< -o $@ $(SRCDIR)/common/system/time.o

monitoring_ext.so:monitoring_ext.o $(SRCDIR)/common/system/time.o	
	$(CC) $(CC_FLAGS) $(PCFLAGS) -shared $< -o $@ $(SRCDIR)/common/system/time.o
monitoring_ext_notime.so:monitoring_ext_notime.o $(SRCDIR)/common/system/time.o	
	$(CC) $(CC_FLAGS) $(PCFLAGS) -shared $< -o $@ $(SRCDIR)/common/system/time.o

######## OPTIONS 

install: modl.ginstall eard_modl.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
