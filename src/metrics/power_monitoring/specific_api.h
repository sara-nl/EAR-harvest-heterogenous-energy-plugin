

#ifdef NO_EARDS
// To be used by external privileged applications
#include <metrics/metrics.h>
#define pm_get_data_size_rapl()	   	RAPL_EVS*sizeof(rapl_data_t)
#define pm_disconnect()          	node_energy_dispose(); 
#define pm_start_rapl() 	        start_rapl_metrics()
#define pm_read_rapl(rm)           	stop_rapl_metrics(rm)
#define pm_node_dc_energy(dc)       read_dc_energy(dc)
#define pm_node_ac_energy(ac)       read_ac_energy(ac)

#endif

#ifdef EARDS
// To be used by ear daemon components
#include <metrics/metrics.h>
#define pm_get_data_size_rapl()     RAPL_EVS*sizeof(rapl_data_t)
#define pm_disconnect()             node_energy_dispose();
#define pm_start_rapl()             start_rapl_metrics()
#define pm_read_rapl(rm)            stop_rapl_metrics(rm)
#define pm_node_dc_energy(dc)       read_dc_energy(dc)
#define pm_node_ac_energy(ac)       read_ac_energy(ac)

#endif

#ifdef EAR_USER

// To be used by EAR library or external applications
#include <common/eards_api.h>
#define pm_get_data_size_rapl() 	eards_get_data_size_rapl()
#define pm_disconnect() 			eards_disconnect()
#define pm_start_rapl()				eards_start_rapl()
#define pm_read_rapl(rm) 			eards_read_rapl(rm)
#define pm_node_dc_energy(dc)		eards_node_dc_energy(dc)
#define pm_node_ac_energy(ac)		eards_node_ac_energy(ac)

#endif
