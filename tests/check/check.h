/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_CHECK_H
#define EAR_CHECK_H

// Searchs the module name in /proc/modules file.
// Returns 1 if found or 0 if doesn't.
int find_module(char *path, char *module);

#endif //EAR_CHECK_H
