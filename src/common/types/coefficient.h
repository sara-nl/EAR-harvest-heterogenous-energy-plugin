/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_TYPES_COEFFICIENT
#define _EAR_TYPES_COEFFICIENT

#include <common/types/generic.h>

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


// Function declarations

// Reads a file of coefficients saved in binary format. A block of memory
// is allocated for this read coefficients, and is returned by the argument
// 'coeffs'. The returned integer is the number of applications read. If the
// integer is negative, one of the following errores ocurred: EAR_ALLOC_ERROR,
// EAR_READ_ERROR or EAR_FILE_NOT_FOUND.
int read_coefficients_file(char *path, coefficient_t **coeffs, int size);

#endif
