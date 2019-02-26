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
#define MAX_POWER_ERROR						0.05
#define MAX_DYNAIS_OVERHEAD					5
#define EAR_OVERHEAD_CONTROL				1
#define MAX_TIME_DYNAIS_WITHOUT_SIGNATURE   30
#define MPI_CALLS_TO_CHECK_PERIODIC         1000
#define PERIOD                              30

#define EAR_PERFORMANCE_TESTS				0
#define MEASURE_DYNAIS_OV					0

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
#define LRZ_POLICY							1
//#define EAR_GUI

// #define USE_EARDBD_CONF	0

#endif
