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

#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
//#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/string_enhanced.h>
#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf_etag.h>
#include <common/types/configuration/cluster_conf_generic.h>



state_t TAG_token(char *token)
{
	if (strcasestr(token,"TAG")!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}

state_t TAG_parse_token(tag_t **tags_i, int *num_tags_i, char *line)
{
    char *buffer_ptr, *second_ptr; //auxiliary pointers
    char *token; //group token
    char *key, *value; //main tokens

    state_t found = EAR_ERROR;

    int i;
    int idx = -1;

    // initial value assignement
    tag_t *tags = *tags_i;
    int num_tags = *num_tags_i;

    if (num_tags == 0)
        tags = NULL;

    token = strtok_r(line, " ", &buffer_ptr);

    while (token != NULL)
    {
        key = strtok_r(token, "=", &second_ptr); 
        value = strtok_r(NULL, "=", &second_ptr);

        if (key == NULL || value == NULL || !strlen(key) || !strlen(value))
        {
            token = strtok_r(NULL, " ", &buffer_ptr);
            warning("Error while parsing tags, continuing to next line\n");
            continue;
        }

        strclean(value, '\n');
        strtoup(key);

        if (!strcmp(key, "TAG"))
        {
            found = EAR_SUCCESS;
            for (i = 0; i < num_tags; i++)
                if (!strcmp(tags[i].id, value)) idx = i;

            if (idx < 0)
            {
                tags = realloc(tags, sizeof(tag_t) * (num_tags + 1));
                idx = num_tags;
                num_tags++;
                memset(&tags[idx], 0, sizeof(tag_t));
                set_default_tag_values(&tags[idx]);
                strcpy(tags[idx].energy_model, "");
                strcpy(tags[idx].energy_plugin, "");
                strcpy(tags[idx].powercap_plugin, "");
                strcpy(tags[idx].id, value);
            }
        }
        //SIMPLE VALUES
        else if (!strcmp(key, "MAX_AVX512"))
        {
            //If there is a comma or the value is less than 10, we assume the freq is specified in GHz
            if (strchr(value, '.') != NULL || atol(value) < 10)
                tags[idx].max_avx512_freq = (ulong)(atof(value) * 1000000);
            else
                tags[idx].max_avx512_freq = (atol(value));
        }
        else if (!strcmp(key, "MAX_AVX2"))
        {
            //If there is a comma or the value is less than 10, we assume the freq is specified in GHz
            if (strchr(value, '.') != NULL || atol(value) < 10)
                tags[idx].max_avx2_freq = (ulong)(atof(value) * 1000000);
            else
                tags[idx].max_avx2_freq = (atol(value));
        }
        else if (!strcmp(key, "MAX_POWER"))
        {
            tags[idx].max_power = atol(value);
        }
        else if (!strcmp(key, "MIN_POWER"))
        {
            tags[idx].min_power = atol(value);
        }
        else if (!strcmp(key, "ERROR_POWER"))
        {
            tags[idx].error_power = atol(value);
        }
        else if (!strcmp(key, "POWERCAP"))
        {
            tags[idx].powercap = atol(value);
        }

        //MODELS
        else if (!strcmp(key, "ENERGY_MODEL"))
        {
            strcpy(tags[idx].energy_model, value);
        }
        else if (!strcmp(key, "ENERGY_PLUGIN"))
        {
            strcpy(tags[idx].energy_plugin, value);
        }
        else if (!strcmp(key, "POWERCAP_PLUGIN"))
        {
            strcpy(tags[idx].powercap_plugin, value);
        }
        else if (!strcmp(key, "COEFFS"))
        {
            strcpy(tags[idx].coeffs, value);
        }

        //TYPE OF POWERCAP AND TAGS -> pending
        else if (!strcmp(key, "DEFAULT"))
        {
            strtoup(value);
            if (!strcmp(value, "YES") || !strcmp(value, "Y"))
                tags[idx].is_default = 1;
            else
                tags[idx].is_default = 0;
        }
        else if (!strcmp(key, "TYPE"))
        {
            strtoup(value);
            if (!strcmp(value, "ARCH"))
                tags[idx].type = TAG_TYPE_ARCH;
        }
        else if (!strcmp(key, "POWERCAP_TYPE"))
        {
            strtoup(value);
            if (!strcmp(value, "APP") || !strcmp(value, "APPLICATION"))
                tags[idx].powercap_type = POWERCAP_TYPE_APP;
            if (!strcmp(value, "NODE"))
                tags[idx].powercap_type = POWERCAP_TYPE_NODE;
        }


        token = strtok_r(NULL, " ", &buffer_ptr);
    }            
    
    *tags_i = tags;
    *num_tags_i = num_tags;
    return found;
}

void print_tags_conf(tag_t *tag)
{
    verbosen(VCCONF, "--> Tag: %s\ttype: %d\tdefault: %d\tpowercap_type: %d\n", tag->id, tag->type, tag->is_default, tag->powercap_type);
    verbosen(VCCONF, "\t\tavx512_freq: %lu\tavx2_freq: %lu\tmax_power: %lu\tmin_power: %lu\terror_power: %lu\tpowercap: %lu\n", 
                     tag->max_avx512_freq, tag->max_avx2_freq, tag->max_power, tag->min_power, tag->error_power, tag->powercap);
    verbosen(VCCONF, "\t\tenergy_model: %s\tenergy_plugin: %s\tpowercap_plugin: %s\n", tag->energy_model, tag->energy_plugin, tag->powercap_plugin);
}

