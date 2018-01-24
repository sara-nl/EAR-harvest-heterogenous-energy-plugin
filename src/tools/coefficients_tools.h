/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _COEFFICIENTS_TOOLS_H_
#define _COEFFICIENTS_TOOLS_H_

// This functio allocates memory for a set of coefficients and initializes them from the file. Returns NULL on error. Updates p_states with
// the number of p_states
struct Coefficients_info *read_coefficients_from_file(char *filename,int *p_states);
#else
#endif
