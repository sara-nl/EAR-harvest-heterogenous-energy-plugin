/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/


/**
*    \file shared_configuration.h
*    \brief This file defines the API to create/attach/dettach/release the shared memory area between the EARD and the EARLib. It is used to communicate the EARLib updates 
*	remotelly requested
*/

#ifndef _SHARED_CONF_H
#define _SHARED_CONF_H

#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/types/coefficient.h>

typedef struct settings_conf{
	uint 	user_type;
	uint 	policy;
	ulong 	max_freq;
	ulong	def_freq;
	double 	th;
	earlib_conf_t lib_info;
} settings_conf_t;

typedef struct resched{
	int 	force_rescheduling;
}resched_t;

/*********** SETTINGS configuration *******************/

/** Sets in path the filename for the shared memory area between EARD and EARL
 * @param path (output)
 */
int get_settings_conf_path(char *tmp,char *path);

/** Creates the shared mmemory. It is used by EARD (server)
*	@param ear_conf_path specifies the path (folder) to create the file used by mmap
*/

settings_conf_t * create_settings_conf_shared_area(char * path);

/** Connects with a previously created shared memory region. It is used by EARLib (client)
*	@param ear_conf_path specifies the path (folder) where the mapped file were created
*/
settings_conf_t * attach_settings_conf_shared_area(char * path);

/** Disconnect from a previously connected shared memory region. It is used by EARLib (client)
*/
void dettach_settings_conf_shared_area();

/** Releases a shared memory area previously created. It is used by EARD (server)
*/
void settings_conf_shared_area_dispose(char * path);

/*********** RESCHED ****************************/
/** Sets in path the filename for the shared memory area between EARD and EARL
 *  * @param path (output)
 *   */
int get_resched_path(char *tmp,char *path);

/** Creates the shared mmemory. It is used by EARD (server)
 * *   @param ear_conf_path specifies the path (folder) to create the file used by mmap
 * */

resched_t * create_resched_shared_area(char *path);

/** Connects with a previously created shared memory region. It is used by EARLib (client)
 * *   @param ear_conf_path specifies the path (folder) where the mapped file were created
 * */
resched_t * attach_resched_shared_area(char * path);

/** Disconnect from a previously connected shared memory region. It is used by EARLib (client)
 * */
void dettach_resched_shared_area();

/** Releases a shared memory area previously created. It is used by EARD (server)
 * */
void resched_shared_area_dispose(char * path);

/***************** COEFFICIENTS **********/
int get_coeffs_path(char *tmp,char *path);

coefficient_v3_t * create_coeffs_shared_area(char * path,coefficient_v3_t *coeffs,int size);

coefficient_v3_t * attach_coeffs_shared_area(char * path,int *size);

void coeffs_shared_area_dispose(char * path);

void dettach_coeffs_shared_area();


#endif
