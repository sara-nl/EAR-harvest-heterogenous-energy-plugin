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

#include <stdio.h>
#include <stdlib.h>
#include <common/types/projection.h>

// Projections
void observed_values_fill_cpi(obs_cpi_t *obs, signature_t *sign)
{
	obs->obs_cpi = sign->CPI;
	obs->obs_tpi = sign->TPI;
}

void observed_values_fill_time(obs_time_t *obs, signature_t *sign)
{
	obs->obs_cpi = sign->CPI;
	obs->obs_time = sign->time;
}

void observed_values_fill_power(obs_power_t *obs, signature_t *sign)
{
	obs->obs_tpi = sign->TPI;
	obs->obs_power = sign->DC_power;
}

double proj_project_cpi(obs_cpi_t *obs, coefficient_t *coeff)
{
	return (coeff->D * obs->obs_cpi) +
		   (coeff->E * obs->obs_tpi) +
		   (coeff->F);
}

double proj_project_time(obs_time_t *obs, coefficient_t *coeff, double proj_cpi)
{
	double freq_src = (double) coeff->pstate_ref;
	double freq_dst = (double) coeff->pstate;

	return ((obs->obs_time * proj_cpi) / obs->obs_cpi) *
		   (freq_src / freq_dst);
}

double proj_project_power(obs_power_t *obs, coefficient_t *coeff)
{
	return (coeff->A * obs->obs_power) +
		   (coeff->B * obs->obs_tpi) +
		   (coeff->C);
}