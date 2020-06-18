/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef COMMON_SYSTEM_SYMPLUG_H
#define COMMON_SYSTEM_SYMPLUG_H

#include <dlfcn.h>
#include <common/types.h>
#include <common/states.h>
#include <common/plugins.h>
#include <common/attributes.h>

state_t symplug_join(void *handle, void *calls[], const char *names[], uint n);

state_t symplug_open(char *path, void *calls[], const char *names[], uint n);
state_t symplug_open_lazy(char *path, void *calls[], const char *names[], uint n);


#endif //COMMON_SYSTEM_SYMPLUG_H
