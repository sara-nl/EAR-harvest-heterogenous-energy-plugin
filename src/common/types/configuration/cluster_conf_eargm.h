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

#ifndef _EARGM_CONF_H
#define _EARGM_CONF_H

#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>


#define MAXENERGY   0
#define MAXPOWER    1
#define BASIC   0
#define KILO  1
#define MEGA  2

#define DECON_LIMITS  3

typedef struct eargm_conf
{
  uint  verbose;    /* default 1 */
  uint  use_aggregation; /* Use aggregated metrics.Default 1 */
  ulong t1;       /* default 60 seconds */
  ulong t2;       /* default 600 seconds */
  ulong   energy;     /* mandatory */
  /* PowerCap */
  #if POWERCAP
  ulong power;
  ulong t1_power;
  ulong powercap_mode;  /* 1=auto by default, 0=monitoring_only */
  ulong defcon_power_limit;   /* Percentages from the maximum to execute the action (0..100)*/
  char powercap_action[GENERIC_NAME]; /* Script file for powercap actions */
  #endif
  /****/
  uint  units;      /* 0=J, 1=KJ=default, 2=MJ, or Watts when using Power */
  uint  policy;     /* 0=MaxEnergy (default), 1=MaxPower ( not yet implemented) */
  uint  port;     /* mandatory */
  uint  mode;     /* refers to energy budget mode */
  uint  defcon_limits[3];
  uint  grace_periods;
  char  mail[GENERIC_NAME];
  char  host[GENERIC_NAME];
  char  energycap_action[GENERIC_NAME]; /* This action is execute in any WARNING level */
  uint  use_log;
} eargm_conf_t;


state_t EARGM_token(char *token);
state_t EARGM_parse_token(eargm_conf_t *conf,char *token);
void copy_eargmd_conf(eargm_conf_t *dest,eargm_conf_t *src);
void set_default_eargm_conf(eargm_conf_t *eargmc);
void print_eargm_conf(eargm_conf_t *conf);

#endif
