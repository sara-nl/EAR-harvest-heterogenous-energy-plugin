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

#ifndef EAR_CONFIG_DEV_H
#define EAR_CONFIG_DEV_H
#include <common/config/config_install.h>


/***** DO NOT MODIFY THESE VARIABLES *******/
/* Sets to 0 MI statistics after signature computation*/
#define RESET_STATISTICS_AT_SIGNATURE 1

/**** These variables modifies features under development ****/
/*** EARD ***/
#define POWERCAP 			1
#define POWERCAP_EXT 	0
#define DYN_PAR 			1
//Specifies if specific node propagation can be used
#define NODE_PROP			0
/** Specifies if eard's rapi uses dynamic size commands. Set to 0 if you want 
 * compatibility with old EAR versions. */
#define DYNAMIC_COMMANDS    1


/*** EARL ***/
/* To be removed and set fixed in the code */
#define EARL_RESEARCH 1
/* When set to 1, master processes synchronize at application start to identify
 * the nombre of masters connected and expected. EARL is set to off in case
 * expected!=connected. */
#define EAR_LIB_SYNC  1
/* Still under development */
#define USE_GPUS_IN_EARL 1
#define USE_GPU_LIB (USE_GPUS && USE_GPUS_IN_EARL)

/**** Deprecated. To be removed ****/
#define EAR_CPUPOWER	1
/* Specifies if RAPL msut be read with MSR registers (1) or with PAPI (0) */
#define USE_MSR_RAPL						1
/* When set to 1 , creates a thread in EARD to support application queries apart
 *  *  * from EARL, do not set to 0 except for debug purposes */
#define APP_API_THREAD            1
/* When set to 1 , creates a thread in EARD for powermonitoring, do not set to 0
 *  *  * except for debug purposes */
#define POWERMON_THREAD           1
/* When set to 1 , creates a thread in EARD for external commands, do not set to
 *  *  * 0 except for debug purposes */
#define EXTERNAL_COMMANDS_THREAD        1

#endif //EAR_CONFIG_DEV_H
