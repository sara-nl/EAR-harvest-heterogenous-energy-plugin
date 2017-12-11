DIR    = $(ROOTDIR)/tests
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/daemon

######## FILES

OUTER_OBJS = $(ROOTDIR)/src/daemon/ear_uncores.a \
             $(ROOTDIR)/src/common/ear_arch_type.o

BINARIES = cpu_model       \
           cpu_aperf       \
           cpu_uncores     \
           cpu_uncores_all \
           papi_init       \
           papi_version    \
           papi_comp_available \
           papi_comp_enabled   \
           gsl_version         \
           slurm_version       \
           free_ipmi_version 
 
######## RULES

.PHONY: check

all: $(BINARIES)

papi_init: papi_init.c
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_init papi_init.c $(PAPI_LDFLAGS)

papi_version: papi_version.c
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_version papi_version.c

papi_comp_available: papi_comp_available.c
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_comp_available papi_comp_available.c $(PAPI_LDFLAGS)

papi_comp_enabled: papi_comp_enabled.c
	$(CC) $(CFLAGS) $(PAPI_CFLAGS) -o papi_comp_enabled papi_comp_enabled.c $(PAPI_LDFLAGS)

cpu_vendor: cpu_vendor.c
	$(CC) $(CFLAGS) -o cpu_vendor cpu_vendor.c

cpu_model: cpu_model.c
	$(CC) $(CFLAGS) -o cpu_model cpu_model.c

cpu_aperf: cpu_aperf.c
	$(CC) $(CFLAGS) -o cpu_aperf cpu_aperf.c

cpu_uncores: cpu_uncores.c
	$(CC) $(CFLAGS) -o cpu_uncores cpu_uncores.c $(OUTER_OBJS)

cpu_uncores_all: cpu_uncores_all.c
	$(CC) $(CFLAGS) -o cpu_uncores_all cpu_uncores_all.c $(OUTER_OBJS)

gsl_version: gsl_version.c
	$(CC) $(CFLAGS) $(GSL_CFLAGS) -o gsl_version gsl_version.c

slurm_version: slurm_version.c	
	$(CC) $(CFLAGS) $(SLURM_CFLAGS) -o slurm_version slurm_version.c

free_ipmi_version: free_ipmi_version.c
	$(CC) $(CFLAGS) $(IPMI_CFLAGS) -o free_ipmi_version free_ipmi_version.c

######## OPTIONS

check:
	@./check.sh

clean:
	rm -f $(BINARIES)
	rm -f *.o
	rm -f core.*
