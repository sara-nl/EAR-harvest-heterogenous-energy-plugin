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

#ifndef _EARD_CONF_H
#define _EARD_CONF_H

#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>



typedef struct eard_conf
{ 
  uint verbose;     /* default 1 */
  ulong period_powermon;  /* default 30000000 (30secs) */
  ulong max_pstate;     /* default 1 */
  uint turbo;       /* Fixed to 0 by the moment */
  uint port;        /* mandatory */
  uint use_mysql;     /* Must EARD report to DB */
  uint use_eardbd;    /* Must EARD report to DB using EARDBD */
  uint force_frequencies; /* 1=EARD will force pstates specified in policies , 0=will not */
  uint    use_log;
} eard_conf_t;



state_t EARD_token(char *token);
state_t EARD_parse_token(eard_conf_t *conf,char *token);
void copy_eard_conf(eard_conf_t *dest,eard_conf_t *src);
void set_default_eard_conf(eard_conf_t *eard);
void print_eard_conf(eard_conf_t *conf);

#endif
