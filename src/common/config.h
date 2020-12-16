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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <common/config/config_def.h>
#include <common/config/config_dev.h>
#include <common/config/config_env.h>
#include <common/config/config_install.h>

/* When defined, activates dynamic traces on EARL */
#define EAR_GUI 1
#define USE_DB  DB_MYSQL || DB_PSQL
#define MAX_CPUS_SUPPORTED 128
#define MAX_GPUS_SUPPORTED 4
#define FEAT_ONLY_MASTER 0
#define ONLY_MASTER (FEAT_ONLY_MASTER || !MPI)


#endif
