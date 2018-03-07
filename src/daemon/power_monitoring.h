/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/
#ifndef _POWER_MONITORING_H_
#define _POWER_MONITORING_H_


// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring);
void powermon_mpi_init(int appID);
void powermon_mpi_finalize(int appID);
void powermon_new_job(int appID,uint from_mpi);
void powermon_end_job(int appID);

#else
#endif
