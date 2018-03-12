/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <common/ear_verbose.h>
#include <common/remote_conf.h>
#include <common/shared_configuration.h>
#include <daemon/remote_daemon_api.h>
#include <common/states.h>

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
			VERBOSE_N(0,"new_job command received %d\n",command.my_req.job_id);
			powermon_new_job(command.my_req.job_id,0);		
			break;
		case EAR_RC_END_JOB:
			VERBOSE_N(0,"end_job command received %d\n",command.my_req.job_id);
			powermon_end_job(command.my_req.job_id);
			break;
		case EAR_RC_MAX_FREQ:
			VERBOSE_N(0,"max_freq command received %lu\n",command.my_req.max_freq);
			ack=dynconf_max_freq(command.my_req.max_freq);
			break;
		case EAR_RC_NEW_TH:
			VERBOSE_N(0,"new_th command received %lu\n",command.my_req.th);
			ack=dynconf_set_th(command.my_req.th);
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
