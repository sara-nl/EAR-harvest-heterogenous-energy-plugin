/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <common/config_install.h>

/* These flags configures EARL */
/* When set to 1, turbo is allowed */
#define USE_TURBO							0
/* When set to 1, frequency is set to default when a new loop starts. Otherwise, the current frequency is used till the policy is applied */
#define RESET_FREQ							0
/* When set to 1, environment variables values oversubscribe ear.conf values. Must be set to 0 on production. To be used under development only */
#define EAR_PERFORMANCE_TESTS				0
/* When set to 1, dynais overhead is measured and reported at application end */
#define MEASURE_DYNAIS_OV					0
/* When set to 1, master processes synchronize at application start to identify the nombre of masters connected and expected. EARL is set to off in case expected!=connected */
#define EAR_LIB_SYNC						1
/* Frequencies are shared between master processes. Under development, must be set to 0 in production*/
#define COORDINATE_FREQUENCIES				0


/* These three values are used to configure what is considered valid power values. AVG power less than MIN_SIG_POWER or greather than MAX_SIG_POWER generates a warning and a syslog message when activated. Values greather than MAX_ERROR_POWER are considered an error and not reported into the DB */
#define MIN_SIG_POWER                       70.0
#define MAX_SIG_POWER                       600.0
#define MAX_ERROR_POWER                     700.0



/* These definitions affects to the output generated by services EAR services */
/* When set to 1, some warning messages and written at syslog explicitly */
#define SYSLOG_MSG							1
/* When set to 1, verbose,debug, and error output is sent to eargmd.log file stored at EAR_TMP */
#define EARGMD_FILE_LOG 0
/* When set to 1, verbose,debug, and error output is sent to eard.log file stored at EAR_TMP */
#define EARD_FILE_LOG   0

/* These two options go together. USE_EXT defines if a automatic network extension must be added for inter-nodes communications. Targeted to architectures where hostname returned is not valid to communicate accross nodes. In that case, NW_EXT specified the extension to concatenate to hostname */
#define USE_EXT								0
#define NW_EXT								"-opa"
/* When set to 1, activates the new SupermucNG policy */


#define LRZ_POLICY							0
/* When defined, activates dynamic traces on EARL */
/*#define EAR_GUI */

/* Specifies if RAPL msut be read with MSR registers (1) or with PAPI (0) */
#define USE_MSR_RAPL	0

/** These definitions affects DB configuration */
/* Maximum number of simultaneous DB connection for mysql user commands */
#define MAX_DB_CONNECTIONS 5
/* When set to 1, loops signatures are not reported to the DB to save space */
#define LARGE_CLUSTER                       1
/* When set to 1, some fields are neither created not reported to the DB to save space */
#define DB_SIMPLE                           0
/* When set to 1, avg frequency is reported with periodic metrics */
#define DEMO                                0
/* Use eardbd.conf file to read privileges mysql user and password  or use ear.conf, comment it to use ear.conf */
/* #define USE_EARDBD_CONF	0 */
#define EARDBD_TYPES						7


/** EARD threads selection, do not modify except for debug purposes **/
/* When set to 1 , creates a thread in EARD to support application queries apart from EARL, do not set to 0 except for debug purposes */
#define APP_API_THREAD						1
/* When set to 1 , creates a thread in EARD for powermonitoring, do not set to 0 except for debug purposes */
#define POWERMON_THREAD						1
/* When set to 1 , creates a thread in EARD for external commands, do not set to 0 except for debug purposes */
#define EXTERNAL_COMMANDS_THREAD 			1


/* Default values used when ear.conf is not specifying all the attributes */
#define PERFORMANCE_GAIN                    0.75
#define EAR_ACCEPTED_TH                     0.05
#define EAR_MIN_P_STATE                     3
#define POWERMON_FREQ                       30
#define DAEMON_PORT_NUMBER                  50000
#define DEFAULT_T1                          60
#define DEFAULT_T2                          600
#define MAX_ENERGY                          300
#define EARGM_PORT_NUMBER                   60000
#define DEFAULT_POWER                       275
#define GRACE_T1                            3
#define EAR_OVERHEAD_CONTROL                1
#define MAX_TIME_DYNAIS_WITHOUT_SIGNATURE   30
#define MPI_CALLS_TO_CHECK_PERIODIC         1000
/* Maximum power error supported. It is used to compute the min time to compute a valid signature */
#define MAX_POWER_ERROR                     0.05
/* Maximum dynais overhead supported per iteration */
#define MAX_DYNAIS_OVERHEAD                 5
/* When set to 1, Pieces of code are activated to reduce the overhead introduced by dynais */
#define PERIOD                              30

/* End default values */


#endif
