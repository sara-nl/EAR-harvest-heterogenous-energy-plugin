/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _SIG_PROJ_H_
#define _SIG_PROJ_H_
#include <common/types/application.h>



double sig_power_projection(application_t *my_app,ulong F,uint Fi);
double sig_cpi_projection(application_t *my_app,ulong F,uint Fi);
double sig_time_projection(application_t *my_app,ulong F,uint Fi,double cpi_pr);
uint equal_with_th(double p,double r,double th);

#else
#endif

