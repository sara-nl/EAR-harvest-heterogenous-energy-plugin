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
#include <common/ear_verbose.h> // Obsolete
#include <common/types/projection.h>
#ifndef PROJ_NO_OBSOLETE
#include <control/frequency.h> // Obsolete
#endif

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

/*
 *
 * Obsolete
 *
 */
static projection_t *projections;

uint proj_create_old(uint p_states)
{
	// Projections allocation
    projections = (projection_t *) malloc(sizeof(projection_t) * p_states);

    if (projections == NULL) {
        ear_verbose(0, "EAR: Error allocating memory for perf. projections\n");
        exit(1);
    }
}

projection_t *proj_perf_project_old(ulong f)
{
	#if PROJ_NO_OBSOLETE
    #warning "C Preprocessor got here!"
	return NULL;
	#else
    ear_debug(4,"EAR(%s):: Getting perfprojection for %u, entry %d\n",
              __FILE__,f,frequency_freq_to_pstate(f));

    return &projections[frequency_freq_to_pstate(f)];
    #endif
}

void proj_perf_set_old(int i, double TP, double PP, double CPI)
{
        ear_debug(4,"EAR(%s):: Setting PP for entry %d (%lf,%lf,%lf)\n",__FILE__,i,TP,PP,CPI);

        projections[i].Time=TP;
        projections[i].Power=PP;
        projections[i].CPI=CPI;

}
void proj_perf_reset_old(uint p_states)
{
    ear_debug(4,"EAR(%s) :: ResetperformanceProjection\n",__FILE__);
    int i;
    for (i=0;i<p_states;i++){
        projections[i].Time=0;
        projections[i].Power=0;
        projections[i].CPI=0;
    }
}

double proj_power_old(double power, double tpi,double A,double B, double C)
{
	double pp;
    pp=A*power+B*tpi+C;
    return pp;
}

double proj_cpi_old(double cpi,double tpi,double D,double E, double F)
{
    double cpi_pr;
    cpi_pr=D*cpi+E*tpi+F;
    return cpi_pr;
}

double proj_time_old(ulong F,ulong Fi,double T,double cpi,double cpi_pr)
{
    double timep;

    //TIME(fn) = TIME(f0) * CPI(fn)/CPI(f0) * (f0/fn)
    timep=T*(cpi_pr/cpi)*((double)F/(double)Fi);

    return timep;
}
