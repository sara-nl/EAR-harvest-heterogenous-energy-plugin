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

#ifndef _EARDBD_CONF_H
#define _EARDBD_CONF_H

#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>

typedef struct eardb_conf
{
  uint aggr_time;
  uint insr_time;
  uint tcp_port;
  uint sec_tcp_port;
  uint sync_tcp_port;
    uint mem_size;
    uchar mem_size_types[EARDBD_TYPES];
    uint    use_log;

} eardb_conf_t;



state_t EARDBD_token(char *token);
state_t EARDBD_parse_token(eardb_conf_t *conf,char *token);
void copy_eardbd_conf(eardb_conf_t *dest,eardb_conf_t *src);
void set_default_eardbd_conf(eardb_conf_t *eargmc);
void print_db_manager(eardb_conf_t *conf);

#endif
