

#ifdef EAR_INTERNAL

#include <metrics/metrics.h>
#define pm_connect()            	node_energy_init();init_rapl_metrics()
#define pm_get_data_size_rapl()	   	RAPL_EVS*sizeof(rapl_data_t)
#define pm_disconnect()          	node_energy_dispose(); 
#define pm_start_rapl() 	        start_rapl_metrics()
#define pm_read_rapl(rm)           	stop_rapl_metrics(rm)
#define pm_node_dc_energy(dc)       read_dc_energy(dc)
#define pm_node_ac_energy(ac)       read_ac_energy(ac)



#else

#include <common/eards_api.h>
#define pm_connect()				eards_connect()
#define pm_get_data_size_rapl() 	eards_get_data_size_rapl()
#define pm_disconnect() 			eards_disconnect()
#define pm_start_rapl()				eards_start_rapl()
#define pm_read_rapl(rm) 			eards_read_rapl(rm)
#define pm_node_dc_energy(dc)		eards_node_dc_energy(dc)
#define pm_node_ac_energy(ac)		eards_node_ac_energy(ac)

#endif
