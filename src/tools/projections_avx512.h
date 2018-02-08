/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

#ifndef _PROJECTIONS_AVX512_H
#define _PROJECTIONS_AVX512_H
double power_proj(double power_f0,double tpi_f0,struct Coefficients_info *my_coeffs,struct Coefficients_info *my_coeffs_avx512,double perc_nonavx512,double perc_avx512);
double cpi_proj(double cpi_f0,double tpi_f0,struct Coefficients_info *my_coeffs,struct Coefficients_info *my_coeffs_avx512,double perc_nonavx512,double perc_avx512);
double time_proj(double time_f0,double cpi_proj,double cpi_f0,unsigned long f0,unsigned long fn);
#else
#endif
