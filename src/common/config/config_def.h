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

#ifndef EAR_CONFIG_DEF_H
#define EAR_CONFIG_DEF_H

/* These flags configures EARL */
/* When set to 1, turbo is allowed */
#define USE_TURBO							0
/* When set to 1, frequency is set to default when a new loop starts.
 * Otherwise, the current frequency is used till the policy is applied */
#define RESET_FREQ							0
/* These three values are used to configure what is considered valid power
 * values. AVG power less than MIN_SIG_POWER or greather than MAX_SIG_POWER
 * generates a warning and a syslog message when activated. Values greather
 * than MAX_ERROR_POWER are considered an error and not reported into the DB */
#define MIN_SIG_POWER                       30.0
#define MAX_SIG_POWER                       600.0
#define MAX_ERROR_POWER                     1000.0
#define MAX_TEMP							150
#define MAX_POWER_CAP					0
#define DEF_POWER_CAP					-1
#define POWER_CAP_TYPE				"node"

/* These definitions affects to the output generated by services EAR services
 * When set to 1, some warning messages and written at syslog explicitly */
#define SYSLOG_MSG							1
/* When set to 1, verbose,debug, and error output is sent to eargmd.log file
 * stored at EAR_TMP */
#define EARGMD_FILE_LOG						1
/* When set to 1, verbose,debug, and error output is sent to eard.log file
 * stored at EAR_TMP */
#define EARD_FILE_LOG						1
/** These definitions affects DB configuration */
/* When set to 1, loops signatures are not reported to the DB to save space */
#define LARGE_CLUSTER                       1
/* When set to 1, some fields are neither created not reported to the DB to
 * save space */
#define DB_SIMPLE                           1
/* When set to 1, avg frequency and temp is reported with periodic metrics */
#define DEMO								0
/* Maximum number of simultaneous DB connection for mysql user commands */
#define MAX_DB_CONNECTIONS					20
/* */
#define PERFORMANCE_GAIN                    0.75
#define EAR_ACCEPTED_TH                     0.05
#define EAR_MIN_P_STATE                     4
#define POWERMON_FREQ                       30
#define DAEMON_PORT_NUMBER                  50000
#define DEFAULT_T1                          60
#define DEFAULT_T2                          600
#define MAX_ENERGY                          300
#define EARGM_PORT_NUMBER                   60000
#define DEFAULT_POWER                       275
#define GRACE_T1                            3
#define EAR_OVERHEAD_CONTROL                1
#define MAX_TIME_DYNAIS_WITHOUT_SIGNATURE   15
#define MPI_CALLS_TO_CHECK_PERIODIC         1000
/* Maximum power error supported. It is used to compute the min time to compute
 * a valid signature */
#define MAX_POWER_ERROR                     0.05

/* Maximum dynais overhead supported per iteration */
#define MAX_DYNAIS_OVERHEAD                 5
/* When set to 1, Pieces of code are activated to reduce the overhead introduced
 * by dynais */
#define PERIOD                              10

/** Database Daemon Deaults */
/* Enabling it, the output of EARDBD is sended to the log file stored in TMP */
#define DEF_DBD_FILE_LOG					1
#define DEF_DBD_AGGREGATION_TIME			60
#define DEF_DBD_INSERTION_TIME				30
#define DEF_DBD_SERVER_PORT					4711
#define DEF_DBD_MIRROR_PORT					4712
#define DEF_DBD_SYNCHR_PORT					4713
/* MegaBytes allocated for caching the node metrics. */
#define DEF_DBD_ALLOC_MBS					120

#define MAX_NODE_ENERGY_LOCK_TRIES      10000

/* Number of max tries  */
#define MAX_SOCKET_COMM_TRIES           10000000
/* Seconds between each EARDBD API connection retrys. */
#define EARDBD_API_RETRY_SECS		60


#define MIN_MPI_FOR_LOW_FREQ		20

#define EARGM_DEF_POWERCAP_LIMIT 0
#define EARGM_DEF_T1_POWER 60
#define EARGM_POWERCAP_DEF_MODE 1
#define EARGM_POWERCAP_DEF_ACTION "no_action"
#define EARGM_ENERGYCAP_DEF_ACTION "no_action"
#define EARGM_POWERCAP_DEF_ACTION_LIMIT 90

#define LIMIT_LARGE_JOBS				2
#define RED_PSTATES_LARGE_JOBS 	1

#endif //EAR_CONFIG_DEF_H

