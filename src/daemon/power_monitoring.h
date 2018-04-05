/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

/**
*    \file power_monitoring.h
*    \brief This file offers the API for the periodic power monitoring. It is used by the power_monitoring thread created by EARD
*
*/

#ifndef _POWER_MONITORING_H_
#define _POWER_MONITORING_H_


/** Periodically monitors the node power monitoring. 
*
*	@param frequency_monitoring sample period expressed in usecs. It is dessigned to be executed by a thread
*/
void *eard_power_monitoring(void *frequency_monitoring);

/**  It must be called when EARLib contacts with EARD 
*/

void powermon_mpi_init(int appID);

/**  It must be called when EARLib disconnects from EARD 
*/
void powermon_mpi_finalize(int appID);

/** It must be called at when job starts 
*/

void powermon_new_job(int appID,uint from_mpi);

/** It must be called at when job ends
*/
void powermon_end_job(int appID);

#else
#endif
