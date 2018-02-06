#ifndef _EAR_TYPES_PROJECTION
#define _EAR_TYPES_PROJECTION

#include <generic.h>

typedef struct Coefficients_info
{
    unsigned long pstate;
    unsigned int available;
    // For power projection
    double A;
    double B;
    double C;
    // For CPI projection
    double D;
    double E;
    double F;
} coefficient_t;

typedef struct PerfProjection
{
    double Time;
    double Power;
    double CPI;
} projection_t;

// Declarations
int read_coefficients_file(char *path, coefficient_t **coeffs, int size);

#endif
