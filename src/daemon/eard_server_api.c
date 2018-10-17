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
#include <linux/limits.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <common/types/job.h>
#include <common/ear_verbose.h>
#include <common/states.h>
#include <common/config.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/eard_rapi.h>



static char *__NAME__ = "EARD_API:";
extern char *__HOST__;

// 2000 and 65535
#define DAEMON_EXTERNAL_CONNEXIONS 1

static  int sfd;

// based on getaddrinfo man pages
int create_server_socket(uint port)
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

	sprintf(buff,"%d",port);

   	s = getaddrinfo(NULL, buff, &hints, &result);
    if (s != 0) {
		VERBOSE_N(0,"getaddrinfo fails for port %s (%s)",buff,strerror(errno));
		return EAR_ERROR;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;



        while (bind(sfd, rp->ai_addr, rp->ai_addrlen) != 0){ 
			eard_verbose(0,"Waiting for connection");
			sleep(10);
	    }
            break;                  /* Success */

    }

   	if (rp == NULL) {               /* No address succeeded */
		VERBOSE_N(0,"bind fails for eards server (%s) ",strerror(errno));
		return EAR_ERROR;
    }else{
		VERBOSE_N(1,"socket and bind for erads socket success");
	}

   	freeaddrinfo(result);           /* No longer needed */

   	if (listen(sfd,DAEMON_EXTERNAL_CONNEXIONS)< 0){
		VERBOSE_N(0,"listen eards socket fails (%s)",strerror(errno));
		close(sfd);
 		return EAR_ERROR;
	}
	VERBOSE_N(1,"socket listen ready!");
 	return sfd;
}
int wait_for_client(int s,struct sockaddr_in *client)
{
	int new_sock;
	int client_addr_size;

    client_addr_size = sizeof(struct sockaddr_in);
    new_sock = accept(s, (struct sockaddr *) &client, &client_addr_size);
    if (new_sock < 0){ 
		VERBOSE_N(0,"accept for eards socket fails %s\n",strerror(errno));
		return EAR_ERROR;
	}
	VERBOSE_N(2,"new connection ");
	return new_sock;
}
void close_server_socket(int sock)
{
	close(sock);
}

int read_command(int s,request_t *command)
{
	int ret;
	ret=read(s,command,sizeof(request_t));
	if ((ret<0) || (ret!=sizeof(request_t))){
		VERBOSE_N(0,"Error reading remote command read %d expected %d",ret,sizeof(request_t));
		if (ret<0) VERBOSE_N(0,"errno %s",strerror(errno));	
		command->req=NO_COMMAND;
	}
	return command->req;
}
void send_answer(int s,ulong *ack)
{
	int ret;
	if ((ret=write(s,ack,sizeof(ulong)))!=sizeof(ulong)) VERBOSE_N(0,"Error sending the answer");
	if (ret<0) VERBOSE_N(0,"(%s)",strerror(errno));
}
void propagate_req(request_t *command, int port)
{

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    int ip1, ip2;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
	char buff[50], nextip1[50], nextip2[50]; 

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    gethostname(buff, 50);

   	s = getaddrinfo(buff, NULL, &hints, &result);
    if (s != 0) {
		VERBOSE_N(0,"getaddrinfo fails for port %s (%s)",buff,strerror(errno));
		return;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in*) (rp->ai_addr);
            struct sockaddr_in temp;

            ip1 = ip2 = htonl(saddr->sin_addr.s_addr);
            ip1 += command->node_dist;
            temp.sin_addr.s_addr = ntohl(ip1);
            strcpy(nextip1, inet_ntoa(temp.sin_addr));

            ip2 -= command->node_dist;
            temp.sin_addr.s_addr = ntohl(ip2);
            strcpy(nextip2, inet_ntoa(temp.sin_addr));
        }
    }

    //the next node will propagate the command at half the distance
    command->node_dist /= 2;
    int actual_dist = command->node_dist;
    //connect to first subnode
    int rc = eards_remote_connect(nextip1, port);
    if (rc < 0)
    {
        fprintf(stderr, "Error connecting to node: %s\n", nextip1);
        correct_error(ntohl(ip1), command, port);
    }
    else
    {
        if (!send_command(command)) 
        {
            fprintf(stderr, "Error propagating command to node %s\n", nextip1);
            eards_remote_disconnect();
            correct_error(ntohl(ip1), command, port);
        }
        else eards_remote_disconnect();
    }
    
    command->node_dist = actual_dist;
    //connect to second subnode
    rc = eards_remote_connect(nextip2, port);
    if (rc < 0)
    {
        fprintf(stderr, "Error connecting to node: %s\n", nextip2);
        correct_error(ntohl(ip2), command, port);
    }
    else
    {
        if (!send_command(command)) 
        {
            fprintf(stderr, "Error propagating command to node %s\n", nextip2);
            eards_remote_disconnect();
            correct_error(ntohl(ip2), command, port);
        }
        else eards_remote_disconnect();
    }
}

int propagate_status(request_t *command, int port, status_t **status)
{
    status_t *status1, *status2, *final_status;
    int num_status1, num_status2;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    int ip1, ip2, self_ip;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
	char buff[50], nextip1[50], nextip2[50]; 

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    gethostname(buff, 50);

   	s = getaddrinfo(buff, NULL, &hints, &result);
    if (s != 0) {
		VERBOSE_N(0,"getaddrinfo fails for port %s (%s)",buff,strerror(errno));
		return EAR_ERROR;
    }

   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in*) (rp->ai_addr);
            struct sockaddr_in temp;

            self_ip = ip1 = ip2 = htonl(saddr->sin_addr.s_addr);
            ip1 += command->node_dist;
            temp.sin_addr.s_addr = ntohl(ip1);
            strcpy(nextip1, inet_ntoa(temp.sin_addr));

            ip2 -= command->node_dist;
            temp.sin_addr.s_addr = ntohl(ip2);
            strcpy(nextip2, inet_ntoa(temp.sin_addr));
        }
    }

    if (command->node_dist < 1)
    {
        final_status = calloc(1, sizeof(status_t));
        final_status[0].ip = ntohl(self_ip);
        final_status[0].ok = STATUS_OK;
        *status = final_status;
        return 1;
    }

    //the next node will propagate the command at half the distance
    command->node_dist /= 2;
    int actual_dist = command->node_dist;

    //connect to first subnode
    int rc = eards_remote_connect(nextip1, port);
    if (rc < 0)
    {
        fprintf(stderr, "Error connecting to node: %s\n", nextip1);
        num_status1 = correct_status(ntohl(ip1), command, port, &status1);
    }
    else
    {
        if ((num_status1 = send_status(command, &status1)) < 1)
        {
            fprintf(stderr, "Error propagating command to node %s\n", nextip1);
            eards_remote_disconnect();
            num_status1 = correct_status(ntohl(ip1), command, port, &status1);
        }
        else eards_remote_disconnect();
    }
    
    command->node_dist = actual_dist;
    //connect to second subnode
    rc = eards_remote_connect(nextip2, port);
    if (rc < 0)
    {
        fprintf(stderr, "Error connecting to node: %s\n", nextip2);
        num_status2 = correct_status(ntohl(ip2), command, port, &status2);
    }
    else
    {
        if ((num_status2 = send_status(command, &status2)) < 1)
        {
            fprintf(stderr, "Error propagating command to node %s\n", nextip2);
            eards_remote_disconnect();
            num_status2 = correct_status(ntohl(ip2), command, port, &status2);
        }
        else eards_remote_disconnect();
    }
    
    int total_status = num_status1 + num_status2;
    final_status = calloc(total_status + 1, sizeof(status_t));
    memcpy(final_status, status1, sizeof(status_t)*num_status1);
    memcpy(&final_status[num_status1], status2, sizeof(status_t)*num_status2);
    final_status[total_status].ip = ntohl(self_ip);
    final_status[total_status].ok = STATUS_OK;
    *status = final_status;
    free(status1);
    free(status2);
    return total_status + 1;

}
