/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <assert.h>
#include <papi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int state = PAPI_library_init(PAPI_VER_CURRENT);
    assert(state == PAPI_VER_CURRENT);

    state = PAPI_get_component_index(argv[1]);
    assert(state >= 0);

    return 0;
}
