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



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/limits.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <common/config.h>

#if SHARED_MEMORY
#include <common/ear_verbose.h>
#include <common/types/job.h>
#include <common/remote_conf.h>
#include <common/shared_configuration.h>
#include <common/states.h>
#include <common/remote_conf.h>
#include <daemon/power_monitoring.h>


int create_server_socket();
int wait_for_client(int sockfd,struct sockaddr_in *client);
void close_server_socket(int sock);

int read_command(int s,request_t *command);
void send_answer(int s,ulong *ack);


extern int eard_must_exit;
extern unsigned long eard_max_freq;

int eards_remote_socket,eards_client;
struct sockaddr_in eards_remote_client;
char *my_tmp;

static const char *__NAME__ = "dynamic_conf:";
ear_conf_t *dyn_conf;


void my_sigusr1(int signun)
{
	VERBOSE_N(0,"dyn_conf thread receives sigusr1\n");
	ear_conf_shared_area_dispose(my_tmp);
	close_server_socket(eards_remote_socket);
	pthread_exit(0);
}

void set_sigusr1()
{
    sigset_t set;
    struct  sigaction sa;
    sigemptyset(&set);
    sigaddset(&set,SIGUSR1);
    pthread_sigmask(SIG_UNBLOCK,&set,NULL); // unblocking SIGUSR1 for this thread
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = my_sigusr1;
    sa.sa_flags=0;
    if (sigaction(SIGUSR1, &sa, NULL) < 0)  		
		ear_verbose(0,"eard doing sigaction of signal s=%d, %s\n",SIGUSR1,strerror(errno));

}

ulong max_dyn_freq()
{
	return dyn_conf->max_freq;
}


int dynconf_max_freq(ulong max_freq)
{
	// we must check it is a valid freq: TODO
	dyn_conf->max_freq=max_freq;
	dyn_conf->force_rescheduling=1;
	return EAR_SUCCESS;
}

int dynconf_set_th(ulong th)
{
	// we must check it is a valid th :TODO
	dyn_conf->th=(double)th/100.0;
	dyn_conf->force_rescheduling=1;	
	return EAR_SUCCESS;
}


void process_remote_requests(int clientfd)
{
	request_t command;
	uint req;
	ulong ack=EAR_SUCCESS;
	VERBOSE_N(0,"connection received\n");
	req=read_command(clientfd,&command);
	switch (req){
		case EAR_RC_NEW_JOB:
			VERBOSE_N(0,"new_job command received %d\n",command.my_req.new_job.id);
			powermon_new_job(&command.my_req.new_job,0);		
			break;
		case EAR_RC_END_JOB:
			VERBOSE_N(0,"end_job command received %d\n",command.my_req.end_job.jid);
			powermon_end_job(command.my_req.end_job.jid,command.my_req.end_job.sid);
			break;
		case EAR_RC_MAX_FREQ:
			VERBOSE_N(0,"max_freq command received %lu\n",command.my_req.ear_conf.max_freq);
			ack=dynconf_max_freq(command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_NEW_TH:
			VERBOSE_N(0,"new_th command received %lu\n",command.my_req.ear_conf.th);
			ack=dynconf_set_th(command.my_req.ear_conf.th);
			break;
		default:
			VERBOSE_N(0,"Invalid remote command\n");
	}	
	send_answer(clientfd,&ack);
}


void * eard_dynamic_configuration(void *tmp)
{
	my_tmp=(char *)tmp;
	int change=0;
	
	ear_verbose(0,"dynamic_configration thread created\n");

	set_sigusr1();


	ear_verbose(0,"Creating scoket for remote commands\n");
	eards_remote_socket=create_server_socket();
	if (eards_remote_socket<0){ 
		ear_verbose(0,"Error creating socket\n");
		pthread_exit(0);
	}
	ear_verbose(0,"creating shared memory tmp=%s\n",my_tmp);
	dyn_conf=create_ear_conf_shared_area(my_tmp,eard_max_freq);
	if (dyn_conf==NULL){
		ear_verbose(0,"Error creating shared memory\n");
		pthread_exit(0);
	}
	ear_verbose(0,"shared memory created max_freq %lu th %lf resched %d\n",dyn_conf->max_freq,dyn_conf->th,dyn_conf->force_rescheduling);
	do{
		ear_verbose(0,"waiting for remote commands\n");
		eards_client=wait_for_client(eards_remote_socket,&eards_remote_client);	
		if (eards_client<0){	
			ear_verbose(0,"dyn_conf: wait_for_client returns error\n");
		}else{ 
			process_remote_requests(eards_client);
			close(eards_client);
		}
	}while(eard_must_exit==0);
    ear_verbose(0,"dyn_conf exiting\n");
    ear_conf_shared_area_dispose(my_tmp);
    close_server_socket(eards_remote_socket);
    pthread_exit(0);
	
}

// 2000 and 65535
#define DAEMON_EXTERNAL_CONNEXIONS 1


static  int sfd;
// based on getaddrinfo man pages
int create_server_socket()
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

	sprintf(buff,"%d",DAEMON_PORT_NUMBER);

   	s = getaddrinfo(NULL, buff, &hints, &result);
    if (s != 0) {
		VERBOSE_N(0,"getaddrinfo fails for port %s \n",buff);
		return EAR_ERROR;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

       if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

       close(sfd);
    }

   	if (rp == NULL) {               /* No address succeeded */
		VERBOSE_N(0,"bind fails for eards server\n");
		return EAR_ERROR;
    }else{
		VERBOSE_N(0,"socket and bind for erads socket success\n");
	}

   	freeaddrinfo(result);           /* No longer needed */

   	if (listen(sfd,DAEMON_EXTERNAL_CONNEXIONS)< 0){
		VERBOSE_N(0,"listen eards socket fails\n");
		close(sfd);
 		return EAR_ERROR;
	}
	VERBOSE_N(0,"eards socket listen ready!\n");
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
	VERBOSE_N(0,"eards new connection \n");
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
		VERBOSE_N(0,"Error reading remote command\n");
		if (ret<0) VERBOSE_N(0,"errno %s\n",strerror(errno));	
		command->req=NO_COMMAND;
	}
	return command->req;
}
void send_answer(int s,ulong *ack)
{
	if (write(s,ack,sizeof(ulong))!=sizeof(ulong)) VERBOSE_N(0,"Error sending the answer\n");
}



#endif
