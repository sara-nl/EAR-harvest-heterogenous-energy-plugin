/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef CONFIG_ENVIRONMENT_H
#define CONFIG_ENVIRONMENT_H

// Database
#define VAR_INS_PATH "EAR_INSTALL_PATH"
#define VAR_TMP_PATH "EAR_TMP"
#define VAR_ETC_PATH "EAR_ETC"
#define VAR_APP_NAME "EAR_APP_NAME"
#define VAR_OPT_VERB "EAR_VERBOSE"
#define VAR_OPT_POLI "EAR_POWER_POLICY"
#define VAR_OPT_FREQ "EAR_FREQUENCY"
#define VAR_OPT_PSTA "EAR_P_STATE"
#define VAR_OPT_LERN "EAR_LEARNING_PHASE"
#define VAR_OPT_ETAG "EAR_ENERGY_TAG"
#define VAR_OPT_USDB "EAR_USER_DB_PATHNAME"
#define VAR_OPT_TRAC "SLURM_EAR_TRACE_PATH"
#define VAR_OPT_THRA "EAR_POWER_POLICY_TH"
#define VAR_OPT_THRB "EAR_PERFORMANCE_PENALTY"
#define VAR_OPT_THRC "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN"

//
#define REL_PATH_LIBR "lib"
#define REL_NAME_LIBR "libear"
#define REL_PATH_LOAD "lib/libearld.so"

//
#define HACK_PATH_LIBR "SLURM_HACK_LIBRARY"
#define HACK_FILE_LIBR "SLURM_HACK_LIBRARY_FILE"
#define HACK_FILE_LOAD "SLURM_HACK_LOADER"
#define HACK_FILE_NVML "SLURM_HACK_NVML"
#define FLAG_NAME_LIBR "SLURM_EAR_MPI_VERSION"


#define NULL_JOB_ID getpid()
#define NULL_STEPID (0xfffffffe)
#define NULL_ACCOUNT "NO_SLURM_ACCOUNT"


#endif //CONFIG_ENVIRONMENT_H
