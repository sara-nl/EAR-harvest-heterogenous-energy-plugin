/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <library/api/ear.h>
#include <library/api/ear_mpi.h>
#include <library/policies/policy.h>
#include <common/output/debug.h>

void before_init()
{
	debug("before_init");
}

void after_init()
{
	debug("after_init");
	ear_init();
}

void before_mpi(mpi_call call_type, p2i buf, p2i dest)
{
	debug("before_mpi");
	policy_mpi_init();
	ear_mpi_call(call_type,buf,dest);
}

void after_mpi(mpi_call call_type)
{
	debug("after_mpi");
	policy_mpi_end();
}

void before_finalize()
{
	debug("before_finalize");
	ear_finalize();
}

void after_finalize()
{
	debug("after_finalize");
}
