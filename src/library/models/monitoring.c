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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <library/common/externs.h>
#include <library/models/models.h>


#include <common/types/signature.h>/* defines signature */
#include <daemon/shared_configuration.h> /* defines settings_conf */
#include <library/mpi_intercept/MPI_calls_coded.h> /* defines list of mpi calls */
#include <library/mpi_intercept/MPI_types.h> /* defines list of mpi calls */



void monitoring_init()
{
}
void monitoring_new_loop()
{
}
void monitoring_end_loop()
{
}
ulong monitoring_policy(signature_t *sig,int *ready)
{
	policy_global_reconfiguration();
	*ready=1;
	return EAR_default_frequency;
}
ulong monitoring_policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig)
{
	return 1;
}


ulong monitoring_default_conf(ulong f)
{
	return f;
}


/*
 * To be used by the pplugin
 */
int dyn_monitoring_init(application_t *app,settings_conf_t *conf,uint pstates)
{
	return EAR_SUCCESS;
}
int dyn_monitoring_end()
{
	return EAR_SUCCESS;
}
int dyn_monitoring_new_loop(uint loop_id)
{
	return EAR_SUCCESS;
}
int dyn_monitoring_end_loop(uint loop_id)
{
	return EAR_SUCCESS;
}
int dyn_monitoring_policy(settings_conf_t *conf,uint loop_id,signature_t *sig,ulong *new_freq)
{
	int ret;
	*new_freq=monitoring_policy(sig,&ret);
	if (ret==1) return EAR_SUCCESS;
	else EAR_NOT_READY;
}
int dyn_monitoring_policy_ok(uint loop_id, signature_t *curr_sig, signature_t *last_sig)
{
	return 1;
}
int dyn_monitoring_default_conf(ulong *f)
{
	return EAR_SUCCESS;
}


