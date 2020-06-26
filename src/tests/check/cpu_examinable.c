/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
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
