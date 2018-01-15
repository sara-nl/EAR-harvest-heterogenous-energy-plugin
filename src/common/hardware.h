/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

// Intel models, Based on arch/x86/include/asm/intel-family.h
// Tip: X means E, EP, ED and Server.
#define CPU_UNIDENTIFIED        -1
#define CPU_SANDY_BRIDGE        42
#define CPU_SANDY_BRIDGE_X      45
#define CPU_IVY_BRIDGE          58
#define CPU_IVY_BRIDGE_X        62
#define CPU_HASWELL             60
#define CPU_HASWELL_X           63
#define CPU_BROADWELL		    61
#define CPU_BROADWELL_X         79
#define CPU_BROADWELL_XEON_D	86
#define CPU_SKYLAKE             94
#define CPU_SKYLAKE_X           85
#define CPU_KABYLAKE            158
#define CPU_KNIGHTS_LANDING     87
#define CPU_KNIGHTS_MILL        133

#define INTEL_VENDOR_NAME       "GenuineIntel"
#define AMD_VENDOR_NAME         "AuthenticAMD"

// Returns if the cpu is examinable by this library
int is_cpu_examinable();
// Returns the vendor ID
int get_vendor_id(char *vendor_id);
// Returns the family of the processor
int get_family();
// Returns 1 if the CPU is APERF/MPERF compatible.
int is_aperf_compatible();
// Returns the size of a cache line of the higher cache level.
int get_cache_line_size();
// Returns an EAR architecture index (top).
int get_model();
// Returns if the processor is HTT capable
int is_cpu_hyperthreading_capable();
