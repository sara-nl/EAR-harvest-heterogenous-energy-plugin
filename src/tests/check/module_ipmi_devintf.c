/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <assert.h>
#include <tests/check/check.h>

int main ()
{
	assert(find_module("ipmi_devintf"));
 
    return 0;
}
