/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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
int EAR_VERBOSE_LEVEL=1;


static int eards_remote_connected=0;
static int eards_sfd=-1;
// Verbosity
static const char *__NAME__ = "eards_rem_client";


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

int eards_new_job(int job_id)
{
	request_t command;
	command.req=EAR_RC_NEW_JOB;
	command.my_req.job_id=job_id;
	VERBOSE_N(0,"command %u job_id %d\n",command.req,command.my_req.job_id);
	return send_command(&command);
}
int eards_end_job(int job_id)
{
    request_t command;
    command.req=EAR_RC_END_JOB;
	command.my_req.job_id=job_id;
	VERBOSE_N(0,"command %u job_id %d\n",command.req,command.my_req.job_id);
	return send_command(&command);
}


int eards_set_freq(unsigned long freq)
{
	request_t command;
	command.req=EAR_RC_MAX_FREQ;
    command.my_req.max_freq=freq;
	return send_command(&command);
}
// New th must be passed as % th=0.75 --> 75
int eards_set_th(unsigned long th)
{
    request_t command;
    command.req=EAR_RC_NEW_TH;
    command.my_req.th=th;
    return send_command(&command);
}

int eards_remote_disconnect()
{
	eards_remote_connected=0;
	close(eards_sfd);
	return EAR_SUCCESS;
}
