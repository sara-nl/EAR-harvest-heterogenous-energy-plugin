/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <errno.h>
#include <ear_models/ear_models.h>

/*
Power = A(fn)*Power + B(fn)*TPI + C(fn) 
CPI = D(fn)*CPI + E(fn)*TPI +  F(fn) 
TIME = TIME * CPI(fn)/CPI * (Rf/fn) 
*/


double power_proj(double power_f0,double tpi_f0,struct Coefficients_info *my_coeffs,struct Coefficients_info *my_coeffs_avx512,double perc_nonavx512,double perc_avx512)
{
	double pp,pp_512;
	pp=my_coeffs->A*power_f0+my_coeffs->B*tpi_f0+my_coeffs->C;
	pp_512=my_coeffs_avx512->A*power_f0+my_coeffs_avx512->B*tpi_f0+my_coeffs_avx512->C;
	fprintf(stderr,"POWER %lf = %lf x %lf + %lf x %lf\n",pp*perc_nonavx512+pp_512*perc_avx512,pp,perc_nonavx512,pp_512,perc_avx512);
	return (pp*perc_nonavx512+pp_512*perc_avx512);
	
}
double cpi_proj(double cpi_f0,double tpi_f0,struct Coefficients_info *my_coeffs,struct Coefficients_info *my_coeffs_avx512,double perc_nonavx512,double perc_avx512)
{
	double cpip,cpip_512;
	cpip=my_coeffs->D*cpi_f0+my_coeffs->E*tpi_f0+my_coeffs->F;
	cpip_512=my_coeffs_avx512->D*cpi_f0+my_coeffs_avx512->E*tpi_f0+my_coeffs_avx512->F;
	fprintf(stderr,"CPI %lf = %lf x %lf + %lf x %lf\n",cpip*perc_nonavx512+cpip_512*perc_avx512,cpip,perc_nonavx512,cpip_512,perc_avx512);
	
	return (cpip*perc_nonavx512+cpip_512*perc_avx512);
}
double time_proj(double time_f0,double cpi_proj,double cpi_f0,unsigned long f0,unsigned long fn)
{
	double timep=time_f0*(cpi_proj/cpi_f0)*((double)f0/(double)fn);
	fprintf(stderr,"TIME f=%lu %lf\n",fn,timep);
	return timep;
}
