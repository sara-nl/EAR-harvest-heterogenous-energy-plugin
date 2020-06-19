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
*    \file remote_daemon_client.h
*    \brief This file defines the client side of the remote EARD API
*
* 	 Note:All these funcions applies to a single node . Global commands must be applying by sending commands to all nodes. 
*/

#ifndef _REMOTE_CLIENT_API_INTERNALS_H
#define _REMOTE_CLIENT_API_INTERNALS_H

#include <common/config.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/eard_conf_rapi.h>
#include <common/types/risk.h>

/** Connects with the EARD running in the given nodename. The current implementation supports a single command per connection
*	The sequence must be : connect +  command + disconnect
* 	@param the nodename to connect with
*/
int eards_remote_connect(char *nodename,uint port);

/** Disconnect from the previously connected EARD
*/
int eards_remote_disconnect();

/** Sends the command to the currently connected fd */
int send_command(request_t *command);

/** Sends the command to the currently connected fd but returns an error if it would block the daemon */
int send_non_block_command(request_t *command);

/** Sends data of size size through the open fd*/
int send_data(int fd, size_t size, char *data, int type);

/** Sends the command to all nodes in ear.conf */
void send_command_all(request_t command, cluster_conf_t my_cluster_conf);

/** Corrects a propagation error, sending to the child nodes when the parent isn't responding. */
void correct_error(int target_idx, int total_ips, int *ips, request_t *command, uint port);

request_header_t correct_data_prop(int target_idx, int total_ips, int *ips, request_t *command, uint port, void **data);

/** Recieves data from a previously send command */
request_header_t receive_data(int fd, void **data);

request_header_t process_data(request_header_t data_head, char **temp_data_ptr, char **final_data_ptr, int final_size);

request_header_t data_all_nodes(request_t *command, cluster_conf_t *my_cluster_conf, void **data);


#endif
