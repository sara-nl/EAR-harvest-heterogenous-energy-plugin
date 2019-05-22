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
#include <common/output/debug.h>
#include <common/output/verbose.h>
#include <library/common/externs.h>
#include <library/models/monitoring.h>
#include <library/models/dyn_policies.h>


void set_policy_monitoring(policy_dyn_t *pol)
{
      pol->init=monitoring_init;
      pol->new_loop=monitoring_new_loop;
      pol->end_loop=monitoring_end_loop;
      pol->policy=monitoring_policy;
      pol->policy_ok=monitoring_policy_ok;
      pol->default_conf=monitoring_default_conf;
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
	dest->init=src->init;
	dest->new_loop=src->new_loop;
	dest->end_loop=src->end_loop;
	dest->policy=src->policy;
	dest->policy_ok=src->policy_ok;
	dest->default_conf=src->default_conf;
}

int load_policy(policy_dyn_t *my_policy,char *policy_name)
{
	void    *policy_file;
	policy_dyn_t tmp_policy;
	char *last_error;
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
	*(void **) (&tmp_policy.new_loop)=check_symbol(policy_file,"new_loop");
	*(void **) (&tmp_policy.end_loop)=check_symbol(policy_file,"end_loop");
	*(void **) (&tmp_policy.policy)=check_symbol(policy_file,"policy");
	*(void **) (&tmp_policy.policy_ok)=check_symbol(policy_file,"policy_ok");
	*(void **) (&tmp_policy.default_conf)=check_symbol(policy_file,"default_conf");
	copy_policy_ops(my_policy,&tmp_policy);
	return EAR_SUCCESS;
}
	

