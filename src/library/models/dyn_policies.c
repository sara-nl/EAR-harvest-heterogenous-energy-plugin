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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <library/common/externs.h>
#include <library/models/monitoring.h>
#include <library/models/dyn_policies_operations.h>
#include <library/models/dyn_policies.h>

/*
 *   int (*init)(application_t *app,settings_conf_t *conf,uint pstates);
 *   int (*end)();
 *   int (*new_loop)(uint loop_id);
 *   int (*end_loop)(uint loop_id);
 *   int (*apply)(settings_conf_t *conf,uint loop_id,signature_t *sig);
 *   int (*is_ok)(uint loop_id, signature_t *curr_sig, signature_t *last_sig);
 *   int (*default_conf)(ulong *f);
 *   int (*new_mpi_call)(mpi_call call_type, p2i buf, p2i dest);
 *   int (*end_mpi_call)(mpi_call call_type);
 *   int (*reconfigure)(settings_conf_t *conf);
 */

void set_policy_monitoring(policy_dyn_t *pol)
{
			if (pol==NULL) return;
      pol->init=dyn_monitoring_init;
			pol->end=dyn_monitoring_end;
      pol->new_loop=dyn_monitoring_new_loop;
      pol->end_loop=dyn_monitoring_end_loop;
      pol->apply=dyn_monitoring_policy;
      pol->is_ok=dyn_monitoring_policy_ok;
      pol->default_conf=dyn_monitoring_default_conf;
			pol->new_mpi_call=NULL;
			pol->end_mpi_call=NULL;
			pol->reconfigure=NULL;
			pol->max_tries=dyn_monitoring_max_tries;
}


void *check_symbol(void * handler,char *sym_name)
{
	void * f;
	char *error_msg;
	f=dlsym(handler,sym_name);
	if ((error_msg=dlerror())!=NULL){
		error("Error initializing policy functions %s , %s",sym_name,error_msg);
		return NULL;
	}
	return f;
}

void copy_policy_ops(policy_dyn_t *dest,policy_dyn_t *src)
{
	if ((dest==NULL) || (src==NULL)) return;
	dest->init=src->init;
	dest->end=src->end;
	dest->new_loop=src->new_loop;
	dest->end_loop=src->end_loop;
	dest->apply=src->apply;
	dest->is_ok=src->is_ok;
	dest->default_conf=src->default_conf;
	dest->new_mpi_call=src->new_mpi_call;
	dest->end_mpi_call=src->end_mpi_call;
	dest->reconfigure=src->reconfigure;
	dest->max_tries=src->max_tries;
}

int load_policy(policy_dyn_t *my_policy,char *policy_name)
{
	void    *policy_file;
	policy_dyn_t tmp_policy;
	if (my_policy==NULL){
		error("load_policy and my_policy is NULL");
		return EAR_ERROR;
	}
	set_policy_monitoring(my_policy);
	/* Check file is ok */
	policy_file=dlopen(policy_name,RTLD_LOCAL | RTLD_LAZY);
	if (!policy_file){
		error("Error loading policy plugin %s (%s)",policy_name,dlerror());
		return EAR_ERROR;
	}
	/* Check symbols */
	*(void **) (&tmp_policy.init)=check_symbol(policy_file,"init");
	if (tmp_policy.init==NULL) return EAR_ERROR;
	*(void **) (&tmp_policy.end)=check_symbol(policy_file,"end");
	*(void **) (&tmp_policy.new_loop)=check_symbol(policy_file,"new_loop");
	*(void **) (&tmp_policy.end_loop)=check_symbol(policy_file,"end_loop");
	*(void **) (&tmp_policy.apply)=check_symbol(policy_file,"apply");
	*(void **) (&tmp_policy.is_ok)=check_symbol(policy_file,"is_ok");
	*(void **) (&tmp_policy.default_conf)=check_symbol(policy_file,"default_conf");
	*(void **) (&tmp_policy.new_mpi_call)=check_symbol(policy_file,"new_mpi_call");
	*(void **) (&tmp_policy.end_mpi_call)=check_symbol(policy_file,"end_mpi_call");
	*(void **) (&tmp_policy.reconfigure)=check_symbol(policy_file,"reconfigure");
	copy_policy_ops(my_policy,&tmp_policy);
	return EAR_SUCCESS;
}
