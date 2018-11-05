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
double project_cpi(signature_t *sign, coefficient_t *coeff)
{
	return (coeff->D * sign->CPI) +
		   (coeff->E * sign->TPI) +
		   (coeff->F);
}

double project_time(signature_t *sign, coefficient_t *coeff)
{
	double proj_cpi = project_cpi(sign, coeff);
	double freq_src = (double) coeff->pstate_ref;
	double freq_dst = (double) coeff->pstate;

	return ((sign->time * proj_cpi) / sign->CPI) *
		   (freq_src / freq_dst);
}

double project_power(signature_t *sign, coefficient_t *coeff)
{
	return (coeff->A * sign->DC_power) +
		   (coeff->B * sign->TPI) +
		   (coeff->C);
}

// Inherited
static projection_t *projections;
static int proj_pstates=0;

uint projection_create(uint p_states)
{
	// Projections allocation
	projections = (projection_t *) malloc(sizeof(projection_t) * p_states);

	proj_pstates=p_states;

	if (projections == NULL) {
		//ear_verbose(0, "EAR: Error allocating memory for perf. projections\n");
		exit(1);
	}
}

projection_t *projection_get(int pstate)
{
	if (pstate>proj_pstates) return NULL;
	return &projections[pstate];
}

void projection_set(int i, double TP, double PP)
{
	projections[i].Time=TP;
	projections[i].Power=PP;
}

void projection_reset(uint p_states)
{
	int i;
	for (i=0;i<p_states;i++){
		projections[i].Time=0;
		projections[i].Power=0;
	}
}
