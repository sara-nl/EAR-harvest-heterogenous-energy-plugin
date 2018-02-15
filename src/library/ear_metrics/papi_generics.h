#ifndef EAR_PAPI_MACROS_H
#define EAR_PAPI_MACROS_H

#define PAPI_INIT_TEST(file)                                                            \
	if (PAPI_is_initialized() == PAPI_NOT_INITED) {                                     \
		if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {                  \
			ear_verbose(0, "%s: Error intializing the PAPI library, exiting\n", file);  \
			exit(1);                                                                    \
		}                                                                               \
	}

#endif