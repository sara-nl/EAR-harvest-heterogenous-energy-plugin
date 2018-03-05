CFLAGS = -I$(SRCDIR)

######## FILES

HARDWARE_OBJS = $(SRCDIR)/metrics/custom/hardware_info.o

BANDWIDTH_OBJS = $(SRCDIR)/metrics/custom/bandwidth.a

MODULE_OBJS = $(SRCDIR)/common/string_enhanced.o

BINARIES = cpu_examinable      \
           cpu_aperf           \
           cpu_uncores         \
           cpu_uncores_all     \
           module_acpi_cpufreq \
           papi_init           \
           papi_version        \
           papi_comp_available \
           papi_comp_enabled   \
           gsl_version         \
           free_ipmi_version 
ifdef SLURM_BASE
	BINARIES += slurm_version
endif
 
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

module_acpi_cpufreq: module_acpi_cpufreq.c $(MODULE_OBJS)
	$(CC) $(CFLAGS) -o module_acpi_cpufreq module_acpi_cpufreq.c $(MODULE_OBJS)

cpu_examinable: cpu_examinable.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o cpu_examinable cpu_examinable.c $(HARDWARE_OBJS)

cpu_aperf: cpu_aperf.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o cpu_aperf cpu_aperf.c $(HARDWARE_OBJS)

cpu_uncores: cpu_uncores.c $(OUTER_OBJS)
	$(CC) $(CFLAGS) -o cpu_uncores cpu_uncores.c $(BANDWIDTH_OBJS) $(HARDWARE_OBJS)

cpu_uncores_all: cpu_uncores_all.c
	$(CC) $(CFLAGS) -o cpu_uncores_all cpu_uncores_all.c $(BANDWIDTH_OBJS) $(HARDWARE_OBJS)

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
	rm -f *.o *.a *.so
