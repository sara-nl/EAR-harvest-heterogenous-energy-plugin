#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <metrics/papi/generics.h>

PAPI_hw_info_t *hw_general = NULL;

void metrics_get_app_name(char *app_name)
{
	const PAPI_exe_info_t *prginfo = NULL;

	PAPI_INIT_TEST(__FILE__);

	if ((prginfo = PAPI_get_executable_info()) == NULL)
	{
		ear_verbose(0,"EAR(%s): Executable info not available. Exiting\n", __FILE__);
		exit(2);
	}

	strcpy(app_name, prginfo->fullname);
}

PAPI_hw_info_t metrics_get_hw_info()
{
	//
	PAPI_INIT_TEST(__FILE__);

	// General hardware info by PAPI
	return PAPI_get_hardware_info();
}
