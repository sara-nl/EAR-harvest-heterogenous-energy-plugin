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

#include <common/remote_conf.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/types/job.h>

// Verbosity
static const char *__NAME__ = "eards_rem_client";
int EAR_VERBOSE_LEVEL=1;

static int eards_remote_connected=0;
static int eards_sfd=-1;

// Sends a command to eard
int send_command(request_t *command)
{
	ulong ack;
	int ret;
	VERBOSE_N(0,"Sending command %u\n",command->req);
	if ((ret=write(eards_sfd,command,sizeof(request_t)))!=sizeof(request_t)){
		if (ret<0){ 
			VERBOSE_N(0,"Error sending command %s\n",strerror(errno));
		}else{ 
			VERBOSE_N(0,"Error sending command \n");
		}
	}
	ret=read(eards_sfd,&ack,sizeof(ulong));
	if (ret<0){
		VERBOSE_N(0,"Error receiving ack %d\n",ret);
	}
	return (ret==sizeof(ulong)); // Should we return ack ?
}

// based on getaddrinfo  mas page
int eards_remote_connect(char *nodename)
{
    int client_sock ;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
	char port_number[50]; 	// that size needs to be validated
    int sfd, s, j;

	if (eards_remote_connected) return eards_sfd;
   	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

	sprintf(port_number,"%d",DAEMON_PORT_NUMBER);
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
	eards_remote_connected=1;
	eards_sfd=sfd;
	return sfd;

}

int eards_new_job(application_t *new_job)
{
	request_t command;
	command.req=EAR_RC_NEW_JOB;
	copy_application(&command.my_req.new_job,new_job);
	VERBOSE_N(0,"command %u job_id %d,%d\n",command.req,command.my_req.new_job.job.id,command.my_req.new_job.job.step_id);
	return send_command(&command);
}

int eards_end_job(job_id jid,job_id sid)
{
    request_t command;
    command.req=EAR_RC_END_JOB;
	command.my_req.end_job.jid=jid;
	command.my_req.end_job.sid=sid;
	VERBOSE_N(0,"command %u job_id %d step_id %d\n",command.req,command.my_req.end_job.jid,command.my_req.end_job.sid);
	return send_command(&command);
}

int eards_set_freq(unsigned long freq)
{
	request_t command;
	command.req=EAR_RC_MAX_FREQ;
    command.my_req.ear_conf.max_freq=freq;
	return send_command(&command);
}

// New th must be passed as % th=0.75 --> 75
int eards_set_th(unsigned long th)
{
    request_t command;
    command.req=EAR_RC_NEW_TH;
    command.my_req.ear_conf.th=th;
    return send_command(&command);
}

int eards_remote_disconnect()
{
	eards_remote_connected=0;
	close(eards_sfd);
	return EAR_SUCCESS;
}
