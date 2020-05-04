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
#include <common/colors.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/system/execute.h>
#define SHOW_DEBUGS 1

#define POWERCAP_MON 0



/* This version is a prototype and should be replaced by an INM plugin+GPU commands for powercap */

#define INM_ENABLE_CMD	"ipmitool raw 0x3a 0xc7 0x01"
#define INM_ENABLE_XCC_BRIGE "ipmitool raw 0x06 0x32 0 1"
#define INM_ENABLE_POLICY_CONTROL_CMD "ipmitool -b 0x00 -t 0x2c nm control enable global" 
#define INM_DISABLE_POLICY_CONTROL_CMD "ipmitool -b 0x00 -t 0x2c nm control disable global" 
#define INM_ENABLE_POWERCAP_POLICY_CMD "ipmitool -b 0x00 -t 0x2c nm policy add policy_id 0 domain platform correction hard power %d  trig_lim 1000 stats 30 enable"
#define INM_DISABLE_POWERCAP_POLICY_CMD "ipmitool -b 0x00 -t 0x2c nm policy remove policy_id 0"
#define INM_GET_POWERCAP_POLICY_CMD "NO_CMD"


int do_cmd(char *cmd)
{
  if (strcmp(cmd,"NO_CMD")==0) return 0;
  return 1;
}

state_t inm_disable_powercap_policy()
{
  char cmd[1024];
  debug("1-Disable policy");
  sprintf(cmd,INM_DISABLE_POWERCAP_POLICY_CMD);
  if (do_cmd(cmd)){
  if (execute(cmd)!=EAR_SUCCESS){
    debug("Error executing policy disable");
		return EAR_ERROR;
  }
  }
	return EAR_SUCCESS;
}
state_t inm_disable_powercap_policies()
{
	char cmd[1024];
  debug("2-Disable policy control");
  sprintf(cmd,INM_DISABLE_POLICY_CONTROL_CMD);
  if (do_cmd(cmd)){
  if (execute(cmd)!=EAR_SUCCESS){
    debug("Error disabling policy control");
		return EAR_ERROR;
  }
  }
	return EAR_SUCCESS;

}

state_t inm_enable_powercap_policies()
{
	char cmd[1024];
  /* 3-Enable powercap policy control */
  sprintf(cmd,INM_ENABLE_POLICY_CONTROL_CMD);
  debug("Enable INM policy control");
  if (do_cmd(cmd)){
  if (execute(cmd)!=EAR_SUCCESS){
    debug("Error executing INM CMD Policy control");
    return EAR_ERROR;
  }
  }
	return EAR_SUCCESS;
}
state_t inm_enable()
{
	char cmd[1024];
  /* 1-Enable XCC-Bridge comm */
  debug("Enable XCC-Bridge");
  sprintf(cmd,INM_ENABLE_XCC_BRIGE);
  if (do_cmd(cmd)){
  if (execute(cmd)!=EAR_SUCCESS){
    debug("Error executing INM XCC-bridge");
    return EAR_ERROR;
  }
  }
  /* 2-Enable INM commands */
  debug("Enable INM");
  sprintf(cmd,INM_ENABLE_CMD);
  if (do_cmd(cmd)){
  if (execute(cmd)!=EAR_SUCCESS){
    debug("Error executing INM CMD enable");
    return EAR_ERROR;
  }
  }
	return EAR_SUCCESS;
}

state_t inm_set_powercap_value(uint domain,uint limit)
{
	char cmd[1024];
	char c_date[128];
	state_t ret;
	sprintf(cmd,INM_ENABLE_POWERCAP_POLICY_CMD,limit);
	return execute(cmd);
}

state_t inm_get_powercap_value(uint powercap)
{
	return EAR_SUCCESS;
}

uint inm_is_powercap_policy_enabled(uint pid)
{
	return 0;
}
