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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/limits.h>
#include <common/states.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <global_manager/eargm_rapi.h>
#include <global_manager/eargm_conf_api.h>

int eargm_remote_socket,eargm_client;
struct sockaddr_in eargm_remote_client;

// 2000 and 65535
#define EARGM_EXTERNAL_CONNEXIONS 1

static  int sfd;
// based on getaddrinfo man pages
int create_server_socket(uint use_port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
	char buff[50]; // This must be checked

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

	sprintf(buff,"%u",use_port);

   	s = getaddrinfo(NULL, buff, &hints, &result);
    if (s != 0) {
		error("eargmd:getaddrinfo fails for port %s \n",buff);
		return EAR_ERROR;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

       while (bind(sfd, rp->ai_addr, rp->ai_addrlen) != 0){
		verbose(VGM+1,"Waiting for connection");
		sleep(10);
		}
            break;                  /* Success */

       close(sfd);
    }

   	if (rp == NULL) {               /* No address succeeded */
		error("bind fails for eargm server\n");
		return EAR_ERROR;
    }else{
		verbose(VGM+2,"socket and bind for erads socket success\n");
	}

   	freeaddrinfo(result);           /* No longer needed */

   	if (listen(sfd,EARGM_EXTERNAL_CONNEXIONS)< 0){
		error("listen eargm socket fails\n");
		close(sfd);
 		return EAR_ERROR;
	}
	verbose(VGM+1,"socket listen ready!\n");
 	return sfd;
}
int wait_for_client(int s,struct sockaddr_in *client)
{
	int new_sock;
	int client_addr_size;

    client_addr_size = sizeof(struct sockaddr_in);
    new_sock = accept(s, (struct sockaddr *) &client, &client_addr_size);
    if (new_sock < 0){ 
		error("accept for eargm socket fails %s\n",strerror(errno));
		return EAR_ERROR;
	}
	verbose(VGM,"new connection \n");
	return new_sock;
}
void close_server_socket(int sock)
{
	close(sock);
}

int read_command(int s,eargm_request_t *command)
{
	int ret;
	ret=read(s,command,sizeof(eargm_request_t));
	if ((ret<0) || (ret!=sizeof(eargm_request_t))){
		error("Error reading remote command\n");
		if (ret<0) error("errno %s\n",strerror(errno));
		command->req=NO_COMMAND;
	}
	return command->req;
}
void send_answer(int s,ulong *ack)
{
	if (write(s,ack,sizeof(ulong))!=sizeof(ulong)) error("Error sending the answer\n");
}
