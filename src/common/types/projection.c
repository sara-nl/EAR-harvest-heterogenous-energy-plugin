/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdlib.h>
#include <common/types/projection.h>
#include <control/frequency.h>



// Function declarations
static projection_t *projections;

uint create_projections(uint p_states)
{
	// Projections allocation
    projections = (projection_t *) malloc(sizeof(projection_t) * p_states);

    if (projections == NULL) {
        ear_verbose(0, "EAR: Error allocating memory for perf. projections\n");
        exit(1);
    }
}

projection_t *performance_projection(ulong f)
{
    ear_debug(4,"EAR(%s):: Getting perfprojection for %u, entry %d\n",__FILE__,f,frequency_freq_to_pstate(f));
    return &projections[frequency_freq_to_pstate(f)];
}
void set_performance_projection(int i, double TP, double PP, double CPI)
{
        ear_debug(4,"EAR(%s):: Setting PP for entry %d (%lf,%lf,%lf)\n",__FILE__,i,TP,PP,CPI);
        projections[i].Time=TP;
        projections[i].Power=PP;
        projections[i].CPI=CPI;

}
void reset_performance_projection(uint p_states)
{
    ear_debug(4,"EAR(%s) :: ResetperformanceProjection\n",__FILE__);
    int i;
    for (i=0;i<p_states;i++){
        projections[i].Time=0;
        projections[i].Power=0;
        projections[i].CPI=0;
    }

}



double power_projection(double power, double tpi,double A,double B, double C)
{
	double pp;
    pp=A*power+B*tpi+C;
    return pp;
}

double cpi_projection(double cpi,double tpi,double D,double E, double F)
{
    double cpi_pr;
    cpi_pr=D*cpi+E*tpi+F;
    return cpi_pr;
}
double time_projection(ulong F,ulong Fi,double T,double cpi,double cpi_pr)
{
    double timep;

    //TIME(fn) = TIME(f0) * CPI(fn)/CPI(f0) * (f0/fn)
    timep=T*(cpi_pr/cpi)*((double)F/(double)Fi);

    return timep;
}



