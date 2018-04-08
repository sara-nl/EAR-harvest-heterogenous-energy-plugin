/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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

