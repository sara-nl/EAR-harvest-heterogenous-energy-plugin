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
#include <common/environment.h>
#include <common/types/configuration/cluster_conf_earlib.h>

state_t EARLIB_token(char *token)
{
	if (token==NULL) return EAR_ERROR;
	return EAR_SUCCESS;
}

state_t EARLIB_parse_token(earlib_conf_t *conf,char *token)
{
		state_t found=EAR_ERROR;
	
		if (!strcmp(token, "COEFFICIENTSDIR"))
		{
			found = EAR_SUCCESS;
			token = strtok(NULL, "=");
			token = strtok(token, "\n");
      remove_chars(token, ' ');
			strcpy(conf->coefficients_pathname, token);
		}
    else if (!strcmp(token, "DYNAISLEVELS"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->dynais_levels = atoi(token);
    }
    else if (!strcmp(token, "DYNAISWINDOWSIZE"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->dynais_window = atoi(token);
    }
    else if (!strcmp(token, "DYNAISTIMEOUT"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->dynais_timeout = atoi(token);
    }
    else if (!strcmp(token, "LIBRARYPERIOD"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->lib_period = atoi(token);
    }
    else if (!strcmp(token, "CHECKEARMODEEVERY"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->check_every = atoi(token);
    }


		return found;
}

void copy_ear_lib_conf(earlib_conf_t *dest,earlib_conf_t *src)
{
	if ((dest!=NULL) && (src!=NULL)){
		strcpy(dest->coefficients_pathname,src->coefficients_pathname);
		dest->dynais_levels=src->dynais_levels;
		dest->dynais_window=src->dynais_window;
		dest->dynais_timeout=src->dynais_timeout;
		dest->lib_period=src->lib_period;
		dest->check_every=src->check_every;
	}
}
void print_earlib_conf(earlib_conf_t *conf)
{
    verbosen(VCCONF, "-->Coefficients path: %s\n-->DynAIS levels: %u\n-->DynAIS window size: %u\n",
            conf->coefficients_pathname, conf->dynais_levels, conf->dynais_window);
	verbosen(VCCONF, "-->dynais timeout %u ear period %u check every %u\n",
		conf->dynais_timeout,conf->lib_period,conf->check_every);
}
void set_default_earlib_conf(earlib_conf_t *earlibc)
{
	strcpy(earlibc->coefficients_pathname,DEFAULT_COEFF_PATHNAME);
	earlibc->dynais_levels=DEFAULT_DYNAIS_LEVELS;
	earlibc->dynais_window=DEFAULT_DYNAIS_WINDOW_SIZE;
	earlibc->dynais_timeout=MAX_TIME_DYNAIS_WITHOUT_SIGNATURE;
	earlibc->lib_period=PERIOD;
	earlibc->check_every=MPI_CALLS_TO_CHECK_PERIODIC;
}

