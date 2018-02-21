#ifndef EAR_PAPI_MACROS_H
#define EAR_PAPI_MACROS_H

#include <common/ear_verbose.h>

#define PAPI_INIT_TEST(file) \
    int papi_init; \
    if (PAPI_is_initialized() == PAPI_NOT_INITED) { \
	if ((papi_init = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) \
	{ \
            ear_verbose(0, "%s: ERROR intializing the PAPI library (%s), exiting\n", \
		file, PAPI_strerror(papi_init)); \
            exit(1); \
        } \
    }

#define PAPI_INIT_MULTIPLEX_TEST(file) \
	int papi_multi; \
	if ((papi_multi = PAPI_multiplex_init()) != PAPI_OK) { \
		ear_verbose(0,"%s: WARNING, %s\n", file, PAPI_strerror(papi_multi)); \
	}

//
void metrics_get_app_name(char *app_name);

#endif
