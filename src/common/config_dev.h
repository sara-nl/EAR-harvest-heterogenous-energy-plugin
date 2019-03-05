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

#ifndef EAR_CONFIG_DEV_H
#define EAR_CONFIG_DEV_H

/* When set to 1, environment variables values oversubscribe ear.conf values.
 * Must be set to 0 on production. To be used under development only. */
#define EAR_PERFORMANCE_TESTS				0
/* When set to 1, dynais overhead is measured and reported at application end. */
#define MEASURE_DYNAIS_OV					0
/** PONERLO A 1 EN TOS LAOS Y QUIAR COMENT */
/* When set to 1, master processes synchronize at application start to identify
 * the nombre of masters connected and expected. EARL is set to off in case
 * expected!=connected. */
#define EAR_LIB_SYNC						1
/* Frequencies are shared between master processes. Under development, must be
 * set to 0 in production*/
#define COORDINATE_FREQUENCIES				0
/* When set to 1, activates the new SupermucNG policy */
#define LRZ_POLICY							0
/* Specifies if RAPL msut be read with MSR registers (1) or with PAPI (0) */
#define USE_MSR_RAPL						1
/* When set to 1, avg frequency is reported with periodic metrics */
#define DEMO                                0
/** EARD threads selection, do not modify except for debug purposes **/
/* When set to 1 , creates a thread in EARD to support application queries apart
 * from EARL, do not set to 0 except for debug purposes */
#define APP_API_THREAD						1
/* When set to 1 , creates a thread in EARD for powermonitoring, do not set to 0
 * except for debug purposes */
#define POWERMON_THREAD						1
/* When set to 1 , creates a thread in EARD for external commands, do not set to
 * 0 except for debug purposes */
#define EXTERNAL_COMMANDS_THREAD 			1
/* Just for ear.conf reading. */
#define EARDBD_TYPES						7

#endif //EAR_CONFIG_DEV_H
