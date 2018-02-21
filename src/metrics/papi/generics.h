#ifndef EAR_PAPI_MACROS_H
#define EAR_PAPI_MACROS_H

#define PAPI_INIT_TEST(file) \
    int papi_init_test; \
    if (PAPI_is_initialized() == PAPI_NOT_INITED) \
	{ \
	papi_init_test = PAPI_library_init(PAPI_VER_CURRENT); \
	ear_verbose(0, "%d - %d\n", PAPI_VER_CURRENT, papi_init_test); \
        \
	if (papi_init_test != PAPI_VER_CURRENT) \
	{ \
            ear_verbose(0, "%s: ERROR intializing the PAPI library (%s), exiting\n", \
		file, PAPI_strerror(papi_init_test)); \
            exit(1); \
        } \
    }

#define PAPI_INIT_MULTIPLEX_TEST(file) \
	int papi_multi_test; \
	if ((papi_multi_test = PAPI_multiplex_init()) != PAPI_OK) { \
		ear_verbose(0,"%s: WARNING, %s\n", file, PAPI_strerror(papi_multi_test)); \
	}

//
void metrics_get_app_name(char *app_name);

#endif
