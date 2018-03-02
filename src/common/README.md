# Power Monitoring library

Description
-----------

The power monitoring library offers basic functionality for power/energy monitoring. 

typedef struct energy_mon_data{  
    node_data_t AC_node_energy;  
    node_data_t DC_node_energy;  
    rapl_data_t DRAM_energy[NUM_SOCKETS];  
    rapl_data_t CPU_energy[NUM_SOCKETS];  
}energy_data_t;  


int init_power_ponitoring();  
void end_power_monitoring();  

int read_enegy_data(energy_data_t *acc_energy);  
int diff_energy_data(energy_data_t *init,energy_data_t *end,energy_data_t *diff);  


Requirements
------------

The power monitoring library uses ear services. EAR must be installed and EAR daemon must be running

Brief installation guide
------------------------

This current version is only a simple prototype, it includes a simple make that creates a shared_library. The path must be included in LD_LIBRARY_PATH to use the library


Privileged and not privileged library
-------------------------------------
The same API can be used with or without privileges. For normal users, a non privileged version is offered . EAR daemon must be running. 

Example: ./my_energy_test ./command.sh 

The priviled version must define LD_LIBRARY_PATH

Example: sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH ./my_energy_test_priv ./command.sh

Binaries are generated with different i

* LD_FLAGS (papi and freeipmi libraries must be included -L/home/xjaneas/opt/lib -lpapi -lfreeipmi)

* CFLAGS (-DEAR_INTERNAL needs to be defined to select internal API)

