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
*    \file monitoring.h
*    \brief This file implements the policy API for the monitoring_only policy
*
*/

#ifndef _MONITORING_H
#define _MONITORING_H
#include <common/config.h>
#include <common/types/projection.h>
#include <common/types/signature.h>

#include <common/types/application.h>/* defines application */
#include <daemon/shared_configuration.h> /* defines settings_conf */
#include <library/mpi_intercept/MPI_calls_coded.h> /* defines list of mpi calls */
#include <library/mpi_intercept/MPI_types.h> /* defines list of mpi calls */



void monitoring_init();
void monitoring_new_loop();
void monitoring_end_loop();
ulong monitoring_policy(signature_t *sig,int *ready);
ulong monitoring_policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig);
ulong monitoring_default_conf(ulong f);

/*
 * These functions are included to be used by the policy plugin mechanism
 */

int dyn_monitoring_init(application_t *app,settings_conf_t *conf,uint pstates);
int dyn_monitoring_end();
int dyn_monitoring_new_loop(loop_id_t *loop_id);
int dyn_monitoring_end_loop(loop_id_t *loop_id);
int dyn_monitoring_policy(settings_conf_t *conf,signature_t *sig,ulong *new_freq);
int dyn_monitoring_policy_ok(signature_t *curr_sig, signature_t *last_sig);
int dyn_monitoring_default_conf(ulong *f);
int dyn_monitoring_max_tries();



#else
#endif
