######## FILES

poli_OBJS = \
       policy.o
modl_BINS = \
       min_time.so  \
       min_energy.so \
       min_energy2.so \
       min_time_no_models.so \
       min_energy_no_models.so \
       monitoring.so \
			 min_time_mpi_info.so \
			 min_energy_auto.so 

modl_PATH = $(DESTDIR)/lib/plugins/policies
modl_PERM = 0775

eard_modl_BINS = \
			min_time_eard.so \
			min_energy_eard.so \
			monitoring_eard.so 

eard_modl_PATH = $(DESTDIR)/lib/plugins/eard_policies
eard_modl_PERM = 0775

########

all: $(poli_OBJS) $(modl_BINS) $(eard_modl_BINS)

min_time_eard_eard.o: min_time_eard.c
	$(CC) $(CC_FLAGS)  $(CFLAGS) -c $<

min_energy_eard.o:min_energy_eard.c
	$(CC) $(CC_FLAGS)  $(CFLAGS) -c $<
monitoring_eard.o:monitoring_eard.c
	$(CC) $(CC_FLAGS)  $(CFLAGS) -c $<

%.o: %.c 
	$(MPICC) $(MPICC_FLAGS)  $(CFLAGS) -c $<

%.so: %.o 
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 	

min_energy_eard.so:min_energy_eard.o $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@   $(SRCDIR)/common/libcommon.a
min_time_eard.so:min_time_eard.o $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@   $(SRCDIR)/common/libcommon.a
monitoring_eard.so:monitoring_eard.o $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@   $(SRCDIR)/common/libcommon.a


min_time_mpi_info.so: min_time_mpi_info.o $(SRCDIR)/common/system/time.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ $(SRCDIR)/common/system/time.o

######## OPTIONS 

install: modl.ginstall eard_modl.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
