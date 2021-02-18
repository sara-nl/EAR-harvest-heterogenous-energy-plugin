
ifeq ($(FEAT_GPU_OPT), 1)
GPU_OPT_FLAG = -DGPU_OPT=1
endif
PCFLAGS = $(CC_FLAGS) $(LIB_CFLAGS) $(CFLAGS) $(GPU_OPT_FLAG)

######## FILES

include ./Makefile.files

poli_DEPS = \
    $(SRCDIR)/common/libcommon.a \
    $(SRCDIR)/daemon/powercap/powercap_status.o
poli_PATH = $(DESTDIR)/lib/plugins/policies
poli_PERM = 0775

pmpi_DEPS = $(SRCDIR)/common/system/time.o

eard_DEPS = $(SRCDIR)/common/libcommon.a
eard_PATH = $(DESTDIR)/lib/plugins/eard_policies
eard_PERM = $(poli_PERM)

########

all: $(poli_OBJS) $(poli_BINS) $(eard_BINS)
 
%.o: %.c 
	$(CC) $(PCFLAGS) -c $<

min_time_eard_eard.o: min_time_eard.c
	$(CC) $(PCFLAGS) -c $<

min_energy_eard.o: min_energy_eard.c
	$(CC) $(PCFLAGS) -c $<

load_balance_eard.o: load_balance_eard.c
	$(CC) $(PCFLAGS) -c $<

monitoring_eard.o: monitoring_eard.c
	$(CC) $(PCFLAGS) -c $<

%.so: %.o $(poli_DEPS)
	$(CC) $(CC_FLAGS) -shared -o $@ $^ 

min_energy_eard.so: min_energy_eard.o $(eard_DEPS)
	$(CC) $(CC_FLAGS) -shared -o $@ $^

load_balance_eard.so: load_balance_eard.o $(eard_DEPS)
	$(CC) $(CC_FLAGS) -shared -o $@ $^

min_time_eard.so: min_time_eard.o $(eard_DEPS)
	$(CC) $(CC_FLAGS) -shared -o $@ $^

monitoring_eard.so: monitoring_eard.o $(eard_DEPS)
	$(CC) $(CC_FLAGS) -shared -o $@ $^

min_time_mpi_info.so: min_time_mpi_info.o $(pmpi_DEPS)
	$(LIBCC) $(LIBCC_FLAGS) -shared -o $@ $^

######## OPTIONS 

install: poli.ginstall eard.ginstall
	ln -sf ./min_energy_eard.so $(eard_PATH)/min_energy_no_models_eard.so 
	ln -sf ./min_time_eard.so $(eard_PATH)/min_time_no_models_eard.so 
ifeq ($(FEAT_RESEARCH),1)
	ln -sf ./min_energy_eard.so $(eard_PATH)/app_min_energy_eard.so 
	ln -sf ./min_time_eard.so $(eard_PATH)/app_min_time_eard.so 
endif

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
