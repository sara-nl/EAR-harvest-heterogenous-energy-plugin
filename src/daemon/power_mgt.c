/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * * 
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * * 
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING  
 * */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <daemon/power_mgt.h>
#include <daemon/inm.h>
#define SHOW_DEBUGS 1




/* This function will load any plugin , detect components etc . It must me executed just once */
state_t pmgt_init()
{
	return EAR_SUCCESS;
}
state_t pmgt_enable(pwr_mgt_t *phandler)
{
	state_t ret;
	ret=inm_enable();
	if (ret!=EAR_SUCCESS)  return ret;
	ret=inm_enable_powercap_policies();
	return ret;
}
state_t pmgt_handler_alloc(pwr_mgt_t **phandler)
{
	*phandler=(pwr_mgt_t*)calloc(1,sizeof(pwr_mgt_t *));
	if (*phandler!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}
state_t pmgt_disable(pwr_mgt_t *phandler)
{
	state_t ret;
	ret=inm_disable_powercap_policy();
	if (ret!=EAR_SUCCESS)  return ret;
	return inm_disable_powercap_policies();	
}
state_t pmgt_set_powercap_value(pwr_mgt_t *phandler,uint domain,uint limit)
{
	return inm_set_powercap_value(domain,limit);
}
state_t pmgt_get_powercap_value(pwr_mgt_t *phandler,uint *powercap)
{
	return inm_get_powercap_value(powercap);
}
uint pmgt_is_powercap_enabled(pwr_mgt_t *phandler,uint pid)
{
	return inm_is_powercap_policy_enabled(pid);
}
void pmgt_print_powercap_value(pwr_mgt_t *phandler,int fd)
{
	inm_print_powercap_value(fd);
}
void pmgt_powercap_to_str(pwr_mgt_t *phandler,char *b)
{
	inm_powercap_to_str(b);
}

