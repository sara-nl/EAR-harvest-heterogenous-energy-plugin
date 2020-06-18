/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <assert.h>
#include <common/hardware/hardware_info.h>

int main ()
{
    int aperf = is_aperf_compatible();
    assert(aperf == 1);
    return 0;
}
