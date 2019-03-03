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

#define PERFORMANCE_GAIN 					0.75
#define EAR_ACCEPTED_TH						0.05
#define EAR_MIN_P_STATE						3
#define USE_TURBO							0
#define RESET_FREQ							0
<<<<<<< HEAD
#define MAX_POWER_ERROR						0.05
#define MAX_DYNAIS_OVERHEAD					5
#define EAR_OVERHEAD_CONTROL				1
#define MAX_TIME_DYNAIS_WITHOUT_SIGNATURE   30
#define MPI_CALLS_TO_CHECK_PERIODIC         1000
#define PERIOD                              30

#define EAR_PERFORMANCE_TESTS				0
#define MEASURE_DYNAIS_OV					0
=======
/** DEV */
/* When set to 1, environment variables values oversubscribe ear.conf values. Must be set to 0 on production. To be used under development only */
#define EAR_PERFORMANCE_TESTS				0
/** DEV */
/* When set to 1, dynais overhead is measured and reported at application end */
#define MEASURE_DYNAIS_OV					0
/** PONERLO A 1 EN TOS LAOS Y QUIAR COMENT */
/* When set to 1, master processes synchronize at application start to identify the nombre of masters connected and expected. EARL is set to off in case expected!=connected */
#define EAR_LIB_SYNC						1
/** DEV */
/* Frequencies are shared between master processes. Under development, must be set to 0 in production*/
#define COORDINATE_FREQUENCIES				0
>>>>>>> 4f1b85cdb716c96b2d38eb9a3ff68ea55f259add

#define POWERMON_FREQ						30
#define DAEMON_PORT_NUMBER					50000

#define DEFAULT_T1							60
#define DEFAULT_T2							600
#define MAX_ENERGY							300
#define EARGM_PORT_NUMBER					60000
#define DEFAULT_POWER						275
#define GRACE_T1							3

#define LARGE_CLUSTER						1
#define DB_SIMPLE							1
#define DEMO								0
#define COEFFS_V3							1
#define APP_API								1

#define MIN_SIG_POWER                       70.0
#define MAX_SIG_POWER                       600.0
#define MAX_ERROR_POWER                     700.0

#define EARDBD_TYPES						7

#define EAR_LIB_SYNC						1
#define COORDINATE_FREQUENCIES				0

#define SYSLOG_MSG							1
#define EARGMD_FILE_LOG 0
#define EARD_FILE_LOG   0

#define USE_EXT								0
#define NW_EXT								"-opa"
<<<<<<< HEAD
=======

/** DEV*/
/* When set to 1, activates the new SupermucNG policy */
>>>>>>> 4f1b85cdb716c96b2d38eb9a3ff68ea55f259add
#define LRZ_POLICY							0
//#define EAR_GUI

<<<<<<< HEAD
// #define USE_EARDBD_CONF	0
=======
/** DEV*/
/* Specifies if RAPL msut be read with MSR registers (1) or with PAPI (0) */
>>>>>>> 4f1b85cdb716c96b2d38eb9a3ff68ea55f259add
#define USE_MSR_RAPL	0

#define MAX_DB_CONNECTIONS 5
<<<<<<< HEAD
=======
/* When set to 1, loops signatures are not reported to the DB to save space */
#define LARGE_CLUSTER                       1
/* When set to 1, some fields are neither created not reported to the DB to save space */
#define DB_SIMPLE                           0
/* When set to 1, avg frequency is reported with periodic metrics */
/** DEV*/
#define DEMO                                0
/** DEV*/
/* Use eardbd.conf file to read privileges mysql user and password  or use ear.conf, comment it to use ear.conf */
/* #define USE_EARDBD_CONF	0 */
/** EARDBD*/
#define EARDBD_TYPES						7

/** DEV*/
/** EARD threads selection, do not modify except for debug purposes **/
/* When set to 1 , creates a thread in EARD to support application queries apart from EARL, do not set to 0 except for debug purposes */
#define APP_API_THREAD						1
/** DEV*/
/* When set to 1 , creates a thread in EARD for powermonitoring, do not set to 0 except for debug purposes */
#define POWERMON_THREAD						1
/** DEV*/
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

>>>>>>> 4f1b85cdb716c96b2d38eb9a3ff68ea55f259add

#endif
