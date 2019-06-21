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


#include <mpi.h>

#include <common/config.h>
#include <library/mpi_intercept/ear_api.h>
#if USE_POLICY_PLUGIN
#include <library/models/dyn_policies_operations.h>
extern policy_dyn_t power_policy;
#endif

void before_init(){
}

void after_init(){
	ear_init();
}
void before_mpi(mpi_call call_type, p2i buf, p2i dest) {
	ear_mpi_call(call_type,buf,dest);
  #if USE_POLICY_PLUGIN
	int ret;
	if (power_policy.new_mpi_call!=NULL){
		if (power_policy.new_mpi_call(call_type,buf,dest)!=EAR_SUCCESS) error("Error in power_policy.new_mpi_call %d",ret);
	}
  #endif
}

void after_mpi(mpi_call call_type){
  #if USE_POLICY_PLUGIN
	int ret;
	if (power_policy.end_mpi_call!=NULL){
		if (power_policy.end_mpi_call(call_type)!=EAR_SUCCESS) error("Error in power_policy.end_mpi_call %d",ret);
	}
  #endif
}

void before_finalize() {
	ear_finalize();
}

void after_finalize() {
}


