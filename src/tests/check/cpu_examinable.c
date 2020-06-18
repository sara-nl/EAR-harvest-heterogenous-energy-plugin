/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <stdio.h>
#include <assert.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/hardware/hardware_info.h>

int main ()
{
    int examinable = is_cpu_examinable();
    verbose(0, "%d", examinable);
    assert(examinable == EAR_SUCCESS);
    return 0;
}
