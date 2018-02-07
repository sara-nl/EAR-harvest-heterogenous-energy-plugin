#ifndef _EAR_TYPES_PROJECTION
#define _EAR_TYPES_PROJECTION

#include <types/generic.h>

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

// Function declarations

// Reads a file of coefficients saved in binary format. A block of memory
// is allocated for this read coefficients, and is returned by the argument
// 'coeffs'. The returned integer is the number of applications read. If the
// integer is negative, one of the following errores ocurred: EAR_ALLOC_ERROR,
// EAR_READ_ERROR or EAR_FILE_NOT_FOUND.
int read_coefficients_file(char *path, coefficient_t **coeffs, int size);

#endif
