#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <metrics/papi/generics.h>
#include <common/ear_verbose.h>

static const char *__NAME__ = "METRICS_GENERICS";

void metrics_get_app_name(char *app_name)
{
	PAPI_exe_info_t *prginfo = NULL;

	PAPI_INIT_TEST(__FILE__);

	if ((prginfo = PAPI_get_executable_info()) == NULL)
	{
		VERBOSE_N(0, "executable info not available, exiting");
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
