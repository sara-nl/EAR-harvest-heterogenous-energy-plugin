/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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
