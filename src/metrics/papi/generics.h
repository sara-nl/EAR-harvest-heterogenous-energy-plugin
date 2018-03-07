#ifndef EAR_PAPI_MACROS_H
#define EAR_PAPI_MACROS_H

#include <papi.h>

// Use along with <ear_verbose.h> and by defining
#define PAPI_INIT_TEST(name) \
    int papi_init; \
    if (PAPI_is_initialized() == PAPI_NOT_INITED) { \
		if ((papi_init = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) { \
			ear_verbose(0, "%s: ERROR intializing the PAPI library (%s), exiting\n", \
				name, PAPI_strerror(papi_init)); \
			exit(1); \
        } \
    }

#define PAPI_INIT_MULTIPLEX_TEST(name) \
	int papi_multi; \
	if ((papi_multi = PAPI_multiplex_init()) != PAPI_OK) { \
		ear_verbose(0,"%s: WARNING, %s\n", name, PAPI_strerror(papi_multi)); \
	}

#define PAPI_GET_COMPONENT(cid, event, name) \
	if ((cid = PAPI_get_component_index(event)) < 0) { \
		fprintf(stderr, "%s: '%s' component not found (%s), exiting\n", \
			name, event, PAPI_strerror(cid)); \
		exit(1); \
	}

// Metrics generic functions
void metrics_get_app_name(char *app_name);
const PAPI_hw_info_t *metrics_get_hw_info();

// PAPI control
int _papi_init();
int _papi_multiplex_init();
int _papi_component_init(char *component_name);
int _papi_event_add(int event_set, char *event_name);
int _papi_counters_stop(int event_set, long long *event_values);
int _papi_counters_read(int event_set, long long *event_values);
int _papi_counters_start(int event_set);

#endif
