/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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



#ifndef _EARD_API_H
#define _EARD_API_H

#define EAR_SUCCESS 			0
#define EAR_ALREADY_CONNECTED 	-1
#define EARDS_NOT_READY			-2

/** Tries to connect with the deamon. Returns 0 on success and -1 otherwise. */
int eards_connect();
/** Closes the connection with the daemon. */
void eards_disconnect();

/** Sends a request to read the RAPL counters. Returns -1 if there's an error,
*   and on success returns 0 and fills the array given by parameter with the
*   counters' values. */
int eards_read_rapl(unsigned long long *values);
/** Sends the request to start the RAPL counters. Returns 0 on success or if
*   the application is not connected, -1 if there's an error. */
int eards_start_rapl();
/** Sends the request to reset the RAPL counters. Returns 0 on success or if
*    the application is not connected, -1 if there's an error. */
int eards_reset_rapl();

/** Requests the rapl data size. Returns -1 if there's an error, and the
*   actual value otherwise. */
unsigned long eards_get_data_size_rapl();
/** Requests the IPMI data size. Returns -1 if there's an error, and the
*   actual value otherwise. */
unsigned long eards_node_energy_data_size();


#else
#endif
