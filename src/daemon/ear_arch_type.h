/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#define CPU_UNIDENTIFIED -1

#define SUPPORTED_ARCH 3

// Returns 1 if the CPU is APERF/MPERF compatible.
int is_aperf_compatible();

// Returns the size of a cache line of the higher cache level.
int get_cache_line_size();

// Returns an EAR architecture index (top).
int get_model();

