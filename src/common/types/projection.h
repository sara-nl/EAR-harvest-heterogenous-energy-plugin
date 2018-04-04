/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_TYPES_PROJECTION
#define _EAR_TYPES_PROJECTION

#include <common/types/generic.h>


typedef struct PerfProjection
{
    double Time;
    double Power;
    double CPI;
} projection_t;


// Function declarations

uint create_projections(uint p_states);

// Given a performance projection and an application signature returns true if
// they are “equal”
uint performance_projection_ok(projection_t *predicted, projection_t *real);
projection_t *performance_projection(ulong f);
void set_performance_projection(int i, double TP, double PP, double CPI);
void reset_performance_projection(uint p_states);

double power_projection(double power, double tpi,double A,double B, double C);
double cpi_projection(double tpi,double cpi,double D,double E, double F);
double time_projection(double F,double Fi,double T,double cpi,double cpi_pr);




#endif
