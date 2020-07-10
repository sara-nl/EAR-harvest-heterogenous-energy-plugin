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

#ifndef METRICS_CPI_INTEL63
#define METRICS_CPI_INTEL63

#include <metrics/cpi/cpi.h>

state_t intel63_cpi_status(topology_t *tp);

state_t intel63_cpi_init(ctx_t *c);

state_t intel63_cpi_dispose(ctx_t *c);

state_t intel63_cpi_reset(ctx_t *c);

state_t intel63_cpi_start(ctx_t *c);

state_t intel63_cpi_stop(ctx_t *c, llong *cycles, llong *insts);

state_t intel63_cpi_read(ctx_t *c, llong *cycles, llong *insts);

#endif //METRICS_CPI_INTEL63
