######## FILES

modl_OBJS = policy.o \
			 min_time.o \
       min_energy.o \
       monitoring.o \
			 min_time_no_models.o \
			 min_energy_no_models.o \
			 dummy.o \
			 empty.o \
			 two_phases.o \
			 min_time_cp.o

modl_BINS = min_time.so min_energy.so monitoring.so min_time_no_models.so min_energy_no_models.so dummy.so empty.so two_phases.so min_time_cp.so
modl_PATH = $(DESTDIR)/lib/plugins/policies
modl_PERM = 0775

########

all: $(modl_OBJS) $(modl_BINS)

%.o: %.c 
	$(MPICC) $(MPICC_FLAGS) -DSHOW_DEBUGS=1 $(CFLAGS) -c $<

%.so: %.o
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 	

######## OPTIONS 

install: modl.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
