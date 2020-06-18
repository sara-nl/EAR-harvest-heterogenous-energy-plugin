/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef COMMON_PLUGINS_H
#define COMMON_PLUGINS_H

#include <common/states.h>

#define empty	NULL
#define none	0

typedef struct ctx_s {
	int initialized;
	void *context;
} ctx_t;

#define preturn(call, ...) \
	if (call == NULL) { \
		return_msg(EAR_UNDEFINED, Generr.api_undefined); \
	} \
	return call (__VA_ARGS__);

#endif //EAR_PRIVATE_PLUGINS_H
