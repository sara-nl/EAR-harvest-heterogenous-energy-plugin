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
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>

#include <common/config.h>
#include <control/frequency.h>
#include <common/types/job.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/ear_verbose.h>
#include <common/states.h>
#include <daemon/eard_server_api.h>
#include <daemon/shared_configuration.h>
#include <daemon/power_monitor.h>
#include <daemon/eard_conf_rapi.h>


extern int eard_must_exit;
extern unsigned long eard_max_freq;

int eards_remote_socket,eards_client;
struct sockaddr_in eards_remote_client;
char *my_tmp;
extern cluster_conf_t my_cluster_conf;
extern my_node_conf_t     *my_node_conf;
extern my_node_conf_t  my_original_node_conf;

static char *__NAME__ = "EARD";
extern char *__HOST__;
extern settings_conf_t *dyn_conf;
extern resched_t *resched_conf;

static ulong* f_list;
static uint num_f;
int last_command = -1;
int last_dist = -1;
int last_command_time = -1;

void print_f_list(uint p_states,ulong *freql)
{
	int i;
	for (i=0;i<p_states;i++) eard_verbose(2,"Freq %u= %lu\n",i,freql[i]);
}

uint is_valid_freq(ulong f, uint p_states,ulong *freql)
{
	int i=0;
	while (i<p_states){
		if (freql[i]==f){
			return 1;
		}
		i++;
	}
	return 0;
	
}
ulong lower_valid_freq(ulong f, uint p_states,ulong *freql)
{
	ulong freq=f-100000;
	while(!is_valid_freq(freq,p_states,freql)) freq=freq-100000;
	if (freq>=freql[p_states-1]){
		eard_verbose(0,"Invalid frequencies selecting the lower one valid: requested %lu selected %lu\n",f,freq);
		return freq;
	}else{
		eard_verbose(0,"PANIC no alternative freq can be selected for %lu\n",f);
		return -1;
	}

}

static void DC_my_sigusr1(int signun)
{
	eard_verbose(1,"thread %u receives sigusr1\n",(uint)pthread_self());
	close_server_socket(eards_remote_socket);
	pthread_exit(0);
}

static void DC_set_sigusr1()
{
    sigset_t set;
    struct  sigaction sa;
    sigfillset(&set);
    sigaddset(&set,SIGUSR1);
    sigaddset(&set,SIGALRM);
    pthread_sigmask(SIG_SETMASK,&set,NULL); // unblocking SIGUSR1 for this thread
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = DC_my_sigusr1;
    sa.sa_flags=0;
    if (sigaction(SIGUSR1, &sa, NULL) < 0)  		
		eard_verbose(0," doing sigaction of signal s=%d, %s\n",SIGUSR1,strerror(errno));

}

ulong max_dyn_freq()
{
	return dyn_conf->max_freq;
}

int dynconf_inc_th(ulong th)
{
	double dth;
	eard_verbose(1,"Increasing th by  %u",th);
	dth=(double)th/100.0;
	if (dyn_conf->policy==MIN_TIME_TO_SOLUTION){
		if (((dyn_conf->th+dth) > 0 ) && ((dyn_conf->th+dth) <=1.0) ){
    		dyn_conf->th=dyn_conf->th+dth;
    		resched_conf->force_rescheduling=1;
		}
	}
	powermon_inc_th(dth);
    return EAR_SUCCESS;

}


int dynconf_max_freq(ulong max_freq)
{
	eard_verbose(1,"Setting max freq to %u",max_freq);
	if (is_valid_freq(max_freq, num_f,f_list)){
		dyn_conf->max_freq=max_freq;
		resched_conf->force_rescheduling=1;
		powermon_new_max_freq(max_freq);
		return EAR_SUCCESS;
	}else{
		
		int freq=lower_valid_freq(max_freq,num_f,f_list);
		if (freq>0){
			dyn_conf->max_freq=freq;
			resched_conf->force_rescheduling=1;
			powermon_new_max_freq(freq);
			return EAR_SUCCESS;
		}else return EAR_ERROR;
	}
}

int dynconf_def_freq(uint p_id,ulong def)
{
	eard_verbose(1,"Setting default freq to %u",def);
    if (is_valid_freq(def,num_f,f_list)){
		if (dyn_conf->policy==p_id){
        	dyn_conf->def_freq=def;
			dyn_conf->def_p_state=frequency_freq_to_pstate(dyn_conf->def_freq);
        	resched_conf->force_rescheduling=1;
		}
        powermon_new_def_freq(p_id,def);
        return EAR_SUCCESS;
    }else{
        int freq=lower_valid_freq(def,num_f,f_list);
        if (freq>0){
			if (dyn_conf->policy==p_id){
            	dyn_conf->def_freq=freq;
				dyn_conf->def_p_state=frequency_freq_to_pstate(dyn_conf->def_freq);
            	resched_conf->force_rescheduling=1;
			}
            powermon_new_def_freq(p_id,freq);
            return EAR_SUCCESS;
        }else return EAR_ERROR;
    }

}

int dynconf_set_freq(ulong freq)
{
	eard_verbose(1,"Setting freq to %u",freq);
	if (is_valid_freq(freq,num_f,f_list)){
		dyn_conf->max_freq=freq;
		dyn_conf->def_freq=freq;
		dyn_conf->def_p_state=frequency_freq_to_pstate(dyn_conf->def_freq);
		resched_conf->force_rescheduling=1;
		powermon_set_freq(freq);
		return EAR_SUCCESS;
	}else{
        int freq2=lower_valid_freq(freq,num_f,f_list);
		if (freq2>0){
			dyn_conf->max_freq=freq2;
			dyn_conf->def_freq=freq2;
			dyn_conf->def_p_state=frequency_freq_to_pstate(dyn_conf->def_freq);
			resched_conf->force_rescheduling=1;
			powermon_set_freq(freq2);
			return EAR_SUCCESS;
		}else return EAR_ERROR;
	}
	


}

int dyncon_restore_conf()
{
	int pid;
	policy_conf_t *my_policy;
	/* We copy the original configuration */
	copy_my_node_conf(my_node_conf,&my_original_node_conf);
	pid=dyn_conf->policy;
	my_policy=get_my_policy_conf(my_node_conf,pid);
	dyn_conf->max_freq=frequency_pstate_to_freq(my_node_conf->max_pstate);
	dyn_conf->def_freq=frequency_pstate_to_freq(my_policy->p_state);
	dyn_conf->def_p_state=my_policy->p_state;
	dyn_conf->th=my_policy->th;
	
	resched_conf->force_rescheduling=1;
	
	return EAR_SUCCESS;
}

int dynconf_red_pstates(uint p_id,uint p_states)
{
	// Reduces p_states both the maximum and the default
	ulong max,def,i;
	uint def_pstate,max_pstate;
	ulong new_def_freq,new_max_freq;
	def_pstate=frequency_freq_to_pstate(dyn_conf->def_freq);
	max_pstate=frequency_freq_to_pstate(dyn_conf->max_freq);
	/* Reducing means incresing in the vector of pstates */
	def_pstate=def_pstate+p_states;
	max_pstate=max_pstate+p_states;
	
	new_def_freq=frequency_pstate_to_freq(def_pstate);
	new_max_freq=frequency_pstate_to_freq(max_pstate);
	

	/* reducing the maximum freq in N p_states */
	dyn_conf->max_freq=new_max_freq;
	dyn_conf->def_freq=new_def_freq;
    resched_conf->force_rescheduling=1;

	/* We must update my_node_info */

	for (i=0;i<TOTAL_POLICIES;i++){
		my_node_conf->policies[i].p_state=my_node_conf->policies[i].p_state-p_states;
	}
	powermon_new_max_freq(new_max_freq);	
}
/** This function is supposed to affect only to MIN_TIME_TO_SOLUTION */
int dynconf_set_th(ulong th)
{
	double dth;
	dth=(double)th/100.0;
	if ((dth > 0 ) && (dth <=1.0 )){
		if (dyn_conf->policy==MIN_TIME_TO_SOLUTION){
			dyn_conf->th=dth;
			resched_conf->force_rescheduling=1;	
		}
		powermon_set_th(dth);
		return EAR_SUCCESS;
	}else return EAR_ERROR;
}

/* This function will propagate the status command and will return the list of node failures */
void dyncon_get_status(int fd, request_t *command)
{
    status_t *status;
    int num_status = propagate_status(command, my_cluster_conf.eard.port, &status);
    if (num_status < 1){
		eard_verbose(0,"Panic propagate_status returns less than 1 status");
        return;
	}
	powermon_get_status(&status[num_status-1]);
    write(fd, &num_status, sizeof(num_status));
    write(fd, status, sizeof(status_t)*num_status);
    free(status);
}


void process_remote_requests(int clientfd)
{
	request_t command;
	uint req;
	long ack=EAR_SUCCESS;
	eard_verbose(1,"connection received\n");
	req=read_command(clientfd,&command);
    if (req != EAR_RC_STATUS && req == last_command && command.time_code == last_command_time)
    {
        eard_verbose(1, "Recieved repeating command: %d", req);
        ack=EAR_IGNORE;
	    send_answer(clientfd,&ack);
        if ((command.node_dist>0) && (command.node_dist != last_dist))
        {
    		last_dist = command.node_dist;
            propagate_req(&command, my_cluster_conf.eard.port);
        }
        return;
    }
    last_dist = command.node_dist;
 	last_command = req;
 	last_command_time = command.time_code;

	switch (req){
		case EAR_RC_NEW_JOB:
			eard_verbose(1,"*******************************************\n");
			eard_verbose(1,"new_job command received %d\n",command.my_req.new_job.job.id);
			powermon_new_job(&command.my_req.new_job,0);		
			break;
		case EAR_RC_END_JOB:
			powermon_end_job(command.my_req.end_job.jid,command.my_req.end_job.sid);
			eard_verbose(1,"end_job command received %d\n",command.my_req.end_job.jid);
			eard_verbose(1,"*******************************************\n");
			break;
		case EAR_RC_MAX_FREQ:
			eard_verbose(1,"max_freq command received %lu\n",command.my_req.ear_conf.max_freq);
			ack=dynconf_max_freq(command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_NEW_TH:
			eard_verbose(1,"new_th command received %lu\n",command.my_req.ear_conf.th);
			ack=dynconf_set_th(command.my_req.ear_conf.th);
			break;
		case EAR_RC_INC_TH:
			eard_verbose(1,"inc_th command received, %lu\n",command.my_req.ear_conf.th);
			ack=dynconf_inc_th(command.my_req.ear_conf.th);
			break;
		case EAR_RC_RED_PSTATE:
			eard_verbose(1,"red_max_and_def_p_state command received\n");
			/* p_id is missing , it is currently applied to all the policies */
			ack=dynconf_red_pstates(command.my_req.ear_conf.p_id,command.my_req.ear_conf.p_states);
			break;
		case EAR_RC_SET_FREQ:
			eard_verbose(1,"set freq command received\n");
			ack=dynconf_set_freq(command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_DEF_FREQ:
			eard_verbose(1,"set def freq command received\n");
			ack=dynconf_def_freq(command.my_req.ear_conf.p_id,command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_REST_CONF:
			eard_verbose(1,"restore conf command received\n");
			ack=dyncon_restore_conf();
			break;
		case EAR_RC_PING:
			eard_verbose(1,"ping received\n");
			break;
		case EAR_RC_STATUS:
			eard_verbose(1,"Status received\n");
			dyncon_get_status(clientfd, &command);
			return;
			break;
		default:
			eard_verbose(0,"Invalid remote command\n");
	}	
	send_answer(clientfd,&ack);
    if (command.node_dist > 0 && req != EAR_RC_PING && req != NO_COMMAND)
    {
        eard_verbose(1, "command=%d propagated distance=%d",req,command.node_dist);
        propagate_req(&command, my_cluster_conf.eard.port);
    }
}
/*
*	THREAD to process remote queries
*/

void * eard_dynamic_configuration(void *tmp)
{
	my_tmp=(char *)tmp;
	int change=0;
	
	eard_verbose(2,"thread created\n");

	DC_set_sigusr1();

	num_f=frequency_get_num_pstates();
	f_list=frequency_get_freq_rank_list();
	print_f_list(num_f,f_list);
	eard_verbose(2,"We have %u valid p_states\n",num_f);
	


	eard_verbose(2,"Creating socket for remote commands\n");
	eards_remote_socket=create_server_socket(my_cluster_conf.eard.port);
	if (eards_remote_socket<0){ 
		eard_verbose(0,"Error creating socket\n");
		pthread_exit(0);
	}
	/*
	*	MAIN LOOP
	*/
	do{
		eard_verbose(2,"waiting for remote commands\n");
		eards_client=wait_for_client(eards_remote_socket,&eards_remote_client);	
		if (eards_client<0){	
			eard_verbose(0," wait_for_client returns error\n");
		}else{ 
			process_remote_requests(eards_client);
			close(eards_client);
		}
	}while(eard_must_exit==0);
    eard_verbose(0,"exiting\n");
    //ear_conf_shared_area_dispose(my_tmp);
    close_server_socket(eards_remote_socket);
    pthread_exit(0);
	
}

