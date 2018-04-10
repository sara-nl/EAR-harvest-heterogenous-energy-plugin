/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <control/frequency.h>
#include <library/common/macros.h>
#include <library/common/externs.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <common/types/coefficient.h>

extern coefficient_t **coefficients;

double sig_power_projection(application_t *my_app,ulong F,uint Fi)
{
	double power,tpi,pp;
	uint Fref;
	power=my_app->DC_power;
	tpi=my_app->TPI;
	// Fref is the index in the pstate list
	// F was ear_frequency
	Fref=frequency_freq_to_pstate(F);
	pp=power_projection(power,tpi,coefficients[Fref][Fi].A,coefficients[Fref][Fi].B,coefficients[Fref][Fi].C);
	return pp;
}

double sig_cpi_projection(application_t *my_app,ulong F,uint Fi)
{
	double cpi,tpi,cpi_pr;
	uint Fref;

	// F was ear_frequency
	Fref=frequency_freq_to_pstate(F);
	tpi=my_app->TPI;
	cpi=my_app->CPI;
	cpi_pr=cpi_projection(cpi,tpi,coefficients[Fref][Fi].D,coefficients[Fref][Fi].E,coefficients[Fref][Fi].F);
	return cpi_pr;
}
double sig_time_projection(application_t *my_app,ulong F,uint Fi,double cpi_pr)
{
	double cpi,my_time,timep;
	uint Fref;
	// F was ear_frequency
	Fref=frequency_freq_to_pstate(F);

	cpi=my_app->CPI;
	my_time=my_app->time;
	timep=time_projection(F,frequency_pstate_to_freq(Fi),my_time,cpi,cpi_pr);
	return timep; 
}

uint equal_with_th(double p,double r,double th)
{
    double diff;
    diff=r-p;
    if (diff<0){// p>r
        diff=diff*-1;
    }
    if (diff<(th*r)) return 1;
    else return 0;
}

