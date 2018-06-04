/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/


/**
*    \file power_monitoring.h
*    \brief This file offers the API for the periodic power monitoring. It is used by the power_monitoring thread created by EARD
*
*/
#ifndef _POWER_MONITORING_H_
#define _POWER_MONITORING_H_

#include <common/types/application.h>

/** Periodically monitors the node power monitoring. 
*
*	@param frequency_monitoring sample period expressed in usecs. It is dessigned to be executed by a thread
*/
void *eard_power_monitoring(void *frequency_monitoring);

/**  It must be called when EARLib contacts with EARD 
*/

void powermon_mpi_init(application_t *j);

/**  It must be called when EARLib disconnects from EARD 
*/
void powermon_mpi_finalize();

/** It must be called at when job starts 
*/

void powermon_new_job(application_t *j,uint from_mpi);

/** It must be called at when job ends
*/
void powermon_end_job(job_id jid,job_id sid);

/** reports the application signature to the power_monitoring module 
*/
void powermon_mpi_signature(application_t *application);

/** if application is not mpi, automatically chages the node freq, it is called by dynamic_configuration API */
void powermon_new_max_freq(ulong maxf);

/** if application is not mpi, automatically chages the node freq, it is called by dynamic_configuration API */
void powermon_new_def_freq(ulong def);

/** Reduces the current freq based on new values. If application is not mpi, automatically chages the node freq if needed */
void powermon_red_freq(ulong max_freq,ulong def_freq);

#endif
