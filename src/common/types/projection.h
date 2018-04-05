/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

/**
*    \file projection.h
*    \brief Projections are used by time&power models. 
*
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

/** Allocates memory to contain the projections for the p_states given by 
*   parameter */ 
uint create_projections(uint p_states);

/** Given a performance projection and an application signature returns true if
*   they are “equal” */
uint performance_projection_ok(projection_t *predicted, projection_t *real);

/** Given a frequency f, returns the projection of the associated p_state of said
*   frequency */
projection_t *performance_projection(ulong f);

/** Sets the values of the performance projection i to the ones given by parameter */
void set_performance_projection(int i, double TP, double PP, double CPI);

/** Resets the projections for power, CPI and time to 0 */
void reset_performance_projection(uint p_states);

/** Creates a prediction for power with the associated formula and returns it
* @return power*A+tpi*B+C
*/
double power_projection(double power, double tpi,double A,double B, double C);


/** Creates a prediction for CPI with the associated formula and returns it
* @return cpi*D+tpi*E+F
*/
double cpi_projection(double tpi,double cpi,double D,double E, double F);

/** Creates a prediction for time with the associated formula and returns it
*	@return T * cpi_pr/cpi * (F/Fi)
*/
double time_projection(ulong F,ulong Fi,double T,double cpi,double cpi_pr);




#endif
