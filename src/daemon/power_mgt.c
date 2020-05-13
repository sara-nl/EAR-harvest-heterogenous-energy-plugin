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
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/symplug.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/power_mgt.h>


typedef struct powercap_symbols {
  state_t (*enable)        ();
  state_t (*disable)       ();
  state_t (*set_powercap_value)(uint pid,uint domain,uint limit);
  state_t (*get_powercap_value)(uint pid,uint *powercap);
  uint    (*is_powercap_policy_enabled)(uint pid);
  void    (*set_status)(uint status);
	void		(*set_pc_mode)(uint mode);
  uint    (*get_powercap_strategy)();
  void    (*powercap_to_str)(char *b);
	void 		(*print_powercap_value)(int fd);
	
} powercapsym_t;

static powercapsym_t pcsyms_fun;
static void *pcsyms_obj = NULL;
const int   pcsyms_n = 10;
extern cluster_conf_t my_cluster_conf;

const char     *pcsyms_names[] ={
  "enable",
  "disable",
  "set_powercap_value",
  "get_powercap_value",
  "is_powercap_policy_enabled",
  "set_status",
	"set_pc_mode",
  "get_powercap_strategy",
  "powercap_to_str",
	"print_powercap_value"
};

#define freturn(call, ...) ((call==NULL)?EAR_UNDEFINED:call(__VA_ARGS__));

#define DEFAULT_PC_PLUGIN_NAME "pc_dvfs"
static uint pc_plugin_loaded=0;

/* This function will load any plugin , detect components etc . It must me executed just once */
state_t pmgt_init()
{
	state_t ret;
	char basic_path[SZ_PATH_INCOMPLETE];
	char *obj_path = getenv("EAR_POWERCAP_POLICY");
  if (obj_path==NULL){
    	sprintf(basic_path,"%s/powercap/%s.so",my_cluster_conf.install.dir_plug,DEFAULT_PC_PLUGIN_NAME);
    	obj_path=basic_path;
	}
	debug("Loading %s powercap plugin",obj_path);
	ret=symplug_open(obj_path, (void **)&pcsyms_fun, pcsyms_names, pcsyms_n);
	return ret;
}
state_t pmgt_enable(pwr_mgt_t *phandler)
{
	state_t ret;
	ret=freturn(pcsyms_fun.enable);
	return ret;
}
state_t pmgt_disable(pwr_mgt_t *phandler)
{
	state_t ret;
	ret=freturn(pcsyms_fun.disable);
	return ret;
}
state_t pmgt_handler_alloc(pwr_mgt_t **phandler)
{
	*phandler=(pwr_mgt_t*)calloc(1,sizeof(pwr_mgt_t *));
	if (*phandler!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}

state_t pmgt_set_powercap_value(pwr_mgt_t *phandler,uint pid,uint domain,uint limit)
{
	state_t ret;
	ret=freturn(pcsyms_fun.set_powercap_value,pid,domain,limit);
	return ret;
}
state_t pmgt_get_powercap_value(pwr_mgt_t *phandler,uint pid,uint *powercap)
{
	state_t ret;
	ret=freturn(pcsyms_fun.get_powercap_value,pid,powercap);
	return ret;
}
uint pmgt_is_powercap_enabled(pwr_mgt_t *phandler,uint pid)
{
	uint ret;
	ret=freturn(pcsyms_fun.is_powercap_policy_enabled,pid);
	return ret;
}
void pmgt_print_powercap_value(pwr_mgt_t *phandler,int fd)
{
	freturn(pcsyms_fun.print_powercap_value,fd);
}
void pmgt_powercap_to_str(pwr_mgt_t *phandler,char *b)
{
	freturn(pcsyms_fun.powercap_to_str,b);
}

void pmgt_set_status(pwr_mgt_t *phandler,uint status)
{
	debug("pmgt_set_status");
	freturn(pcsyms_fun.set_status,status);
}
uint pmgt_get_powercap_strategy(pwr_mgt_t *phandler)
{
	uint ret;
	debug("pmgt_strategy");
	ret=freturn(pcsyms_fun.get_powercap_strategy);
	return ret;
}
void pmgt_set_pc_mode(pwr_mgt_t *phandler,uint mode)
{
	freturn(pcsyms_fun.set_pc_mode,mode);
}


