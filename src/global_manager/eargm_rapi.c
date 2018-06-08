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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h> 
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <global_manager/eargm_conf_api.h>
#include <global_manager/eargm_rapi.h>
#include <common/ear_verbose.h>
#include <common/states.h>

// Verbosity
static const char *__NAME__ = "eargm_client_api";

extern int EAR_VERBOSE_LEVEL;
static int eargm_sfd;
static uint eargm_remote_connected=0;

static int send_command(eargm_request_t *command)
{
	ulong ack;
	int ret;
	VERBOSE_N(2,"Sending command %u\n",command->req);
	if (!eargm_remote_connected) return 0;
	if ((ret=write(eargm_sfd,command,sizeof(eargm_request_t)))!=sizeof(eargm_request_t)){
		if (ret<0){ 
			VERBOSE_N(0,"Error sending command %s\n",strerror(errno));
		}else{ 
			VERBOSE_N(0,"Error sending command \n");
		}
	}
	ret=read(eargm_sfd,&ack,sizeof(ulong));
	if (ret<0){
		VERBOSE_N(0,"Error receiving ack %d\n",ret);
	}
	return (ret==sizeof(ulong)); // Should we return ack ?
}

// based on getaddrinfo  man page
int eargm_connect(char *nodename,uint use_port)
{
    int client_sock ;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
	char port_number[50]; 	// that size needs to be validated
    int sfd, s, j;

	if (eargm_remote_connected) return eargm_sfd;
   	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

	sprintf(port_number,"%u",use_port);
   	s = getaddrinfo(nodename, port_number, &hints, &result);
    if (s != 0) {
		printf("getaddrinfo failt for %s and %s\n",nodename,port_number);
		return EAR_ERROR;
    }

   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1) // if we can not create the socket we continue
            continue;

       if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

       close(sfd);
    }

   	if (rp == NULL) {               /* No address succeeded */
		printf("Failing in connecting to remote erds\n");
		return EAR_ERROR;
    }

   	freeaddrinfo(result);           /* No longer needed */
	eargm_remote_connected=1;
	eargm_sfd=sfd;
	return sfd;

}

int eargm_new_job(uint num_nodes)
{
	eargm_request_t command;
	command.req=EARGM_NEW_JOB;
	command.num_nodes=num_nodes;
	VERBOSE_N(2,"command %u num_nodes %u\n",command.req,command.num_nodes);
	return send_command(&command);
}

int eargm_end_job(uint num_nodes)
{
    eargm_request_t command;
    command.req=EARGM_END_JOB;
	command.num_nodes=num_nodes;
	VERBOSE_N(2,"command %u num_nodes %u\n",command.req,command.num_nodes);
	return send_command(&command);
}

int eargm_disconnect()
{
	eargm_remote_connected=0;
	close(eargm_sfd);
	return EAR_SUCCESS;
}
