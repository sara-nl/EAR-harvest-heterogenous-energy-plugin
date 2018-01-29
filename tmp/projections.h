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

#ifndef _PROJECTIONS_H_
#define _PROJECTIONS_H_
double power_proj(double power_f0,double tpi_f0,struct Coefficients_info *my_coeffs);
double cpi_proj(double cpi_f0,double tpi_f0,struct Coefficients_info *my_coeffs);
double time_proj(double time_f0,double cpi_proj,double cpi_f0,unsigned long f0,unsigned long fn);
#else
#endif
