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

#ifndef _TAG_CONF_H
#define _TAG_CONF_H

#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>

typedef struct tags
{
    char id[GENERIC_NAME];
    char type;
    char is_default;
    char powercap_type;
    ulong max_avx512_freq;
    ulong max_avx2_freq;
    ulong max_power;
    ulong min_power;
    ulong max_temp;
    ulong error_power;
    long powercap;
    char energy_model[GENERIC_NAME];
    char energy_plugin[GENERIC_NAME];
    char powercap_plugin[GENERIC_NAME];
    char coeffs[GENERIC_NAME];
} tag_t;


state_t TAG_token(char *token);
state_t TAG_parse_token(tag_t **tags_i, int *num_tags_i, char *line);
void print_tags_conf(tag_t *tag);


#endif
