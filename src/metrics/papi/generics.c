#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <metrics/papi/generics.h>
#include <common/ear_verbose.h>

static const char *__NAME__ = "PAPI_GENERICS";

void metrics_get_app_name(char *app_name)
{
	const PAPI_exe_info_t *prginfo = NULL;

	PAPI_INIT_TEST(__FILE__);

	if ((prginfo = PAPI_get_executable_info()) == NULL)
	{
		VERBOSE_N(0, "executable info not available, exiting");
		exit(2);
	}

	strcpy(app_name, prginfo->fullname);
}

const PAPI_hw_info_t *metrics_get_hw_info()
{
	//
	PAPI_INIT_TEST(__FILE__);

	// General hardware info by PAPI
	return PAPI_get_hardware_info();
}

/*
 *
 * PAPI control (not static, but internal)
 *
 */

int _papi_start_counters(int event_set)
{
	int result;

	if ((result = PAPI_start(event_set) != PAPI_OK)) {
		VERBOSE_N(0, "ERROR while starting accounting (%s)", PAPI_strerror(result));
		return 0;
	}

	return 1;
}

int _papi_stop_counters(int event_set, long long *event_values)
{
	int result;

	if ((result = PAPI_stop(event_set, event_values)) != PAPI_OK) {
		VERBOSE_N(0, "ERROR while stopping accounting (%s)", PAPI_strerror(result));
		return 0;
	}
	return 1;
}

int _papi_read_counters(int event_set, long long *event_values)
{
	int result;

	if ((result = PAPI_read(event_set, event_values)) != PAPI_OK) {
		VERBOSE_N(0, "ERROR while reading counters (%s)", PAPI_strerror(result));
		return 0;
	}
	return 1;
}

int _papi_event_add(int event_set, char *event_name)
{
	int result;

	if ((result = PAPI_add_named_event(event_set, event_name)) != PAPI_OK) {
		VERBOSE_N(0, "ERROR while adding %s event (%s)", event_name, PAPI_strerror(result));
		return 0;
	}
	return 1;
}

int _papi_multiplex_init()
{
	int papi_multi;

	if ((papi_multi = PAPI_multiplex_init()) != PAPI_OK){
		VERBOSE_N(0,"WARNING when multiplexing %s", name, PAPI_strerror(papi_multi));
		return 0;
	}
	return 1;
}

int _papi_component_init(char *component_name)
{
	const PAPI_component_info_t *info = NULL;
	int cid;

	if ((cid = PAPI_get_component_index(component_name)) < 0) {
		VERBOSE_N(0, "ERROR, %s component not found (%s)", component_name, PAPI_strerror(cid));
		return 0;
	}
	if ((info = PAPI_get_component_info(cid)) == NULL) {
		VERBOSE_N(0, "ERROR, error while accessing to component %s info", component_name);
		return 0;
	}
	if (info->disabled) {
		VERBOSE_N(0, "ERROR, %s disabled (%s)", component_name, info->disabled_reason);
		return 0;
	}

	DEBUG_F(0, "PAPI component %s has been initialized correctly", component_name);
	return 1;
}

int _papi_init()
{
	int papi_init;

	if (PAPI_is_initialized() != PAPI_NOT_INITED) {
		return 1;
	}

	if ((papi_init = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
		VERBOSE_N(0, "ERROR intializing PAPI (%s), exiting", PAPI_strerror(papi_init));
		exit(1);
	}

	DEBUG_F(0, "PAPI has been initialized correctly");
	return 1;
}