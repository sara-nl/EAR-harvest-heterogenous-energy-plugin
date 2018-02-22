#ifndef EAR_PAPI_MACROS_H
#define EAR_PAPI_MACROS_H

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

//
void metrics_get_app_name(char *app_name);
PAPI_hw_info_t metrics_get_hw_info();

#endif
