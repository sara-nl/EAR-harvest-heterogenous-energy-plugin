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

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <linux/limits.h>

#include <common/config.h>
#include <common/types/job.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/output/verbose.h>
#include <common/states.h>
#include <daemon/eard_server_api.h>
#include <daemon/shared_configuration.h>
#include <daemon/power_monitor.h>
#include <daemon/eard_conf_rapi.h>
#include <control/frequency.h>

extern int eard_must_exit;
extern unsigned long eard_max_freq;
extern policy_conf_t default_policy_context;
extern cluster_conf_t my_cluster_conf;
extern my_node_conf_t *my_node_conf;
extern my_node_conf_t my_original_node_conf;
extern settings_conf_t *dyn_conf;
extern resched_t *resched_conf;

int eards_remote_socket, eards_client;
struct sockaddr_in eards_remote_client;
char *my_tmp;
static ulong *f_list;
static uint num_f;
int last_command = -1;
int last_dist = -1;
int last_command_time = -1;

static char *TH_NAME = "RemoteAPI";
static ehandler_t my_eh_rapi;

void print_f_list(uint p_states, ulong *freql) {
	int i;
	for (i = 0; i < p_states; i++) {
		verbose(VCONF + 2, "Freq %u= %lu\n", i, freql[i]);
	}
}

uint is_valid_freq(ulong f, uint p_states, ulong *f_list) {
	int i = 0;

	while (i < p_states) {
		if (f_list[i] == f) {
			return 1;
		}
		i++;
	}
	return 0;

}

ulong lower_valid_freq(ulong f, uint p_states, ulong *f_list) {
	uint i = 0, found = 0;
	while (!found && (i < p_states)) {
		if (f_list[i] < f) found = 1;
		else i++;
	}
	if (found) return f_list[i];
	else return f_list[default_policy_context.p_state];

}

static void DC_my_sigusr1(int signun) {
	verbose(VCONF, "thread %u receives sigusr1\n", (uint) pthread_self());
	close_server_socket(eards_remote_socket);
	pthread_exit(0);
}

static void DC_set_sigusr1() {
	sigset_t set;
	struct sigaction sa;
	sigfillset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_SETMASK, &set, NULL); // unblocking SIGUSR1 for this thread
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = DC_my_sigusr1;
	sa.sa_flags = 0;
	if (sigaction(SIGUSR1, &sa, NULL) < 0)
		error(" doing sigaction of signal s=%d, %s\n", SIGUSR1, strerror(errno));

}

ulong max_dyn_freq() {
	return dyn_conf->max_freq;
}

int dynconf_inc_th(uint p_id, ulong th) {
	double dth;
	verbose(VCONF,"Increasing th by  %lu",th);
	dth=(double)th/100.0;
	if (dyn_conf->policy==p_id){
		if (((dyn_conf->settings[0]+dth) > 0 ) && ((dyn_conf->settings[0]+dth) <=1.0) ){
    		dyn_conf->settings[0]=dyn_conf->settings[0]+dth;
    		resched_conf->force_rescheduling=1;
		}
	}
	powermon_inc_th(p_id, dth);
	return EAR_SUCCESS;

}


int dynconf_max_freq(ulong max_freq) {
	verbose(VCONF, "Setting max freq to %lu", max_freq);
	if (is_valid_freq(max_freq, num_f, f_list)) {
		dyn_conf->max_freq = max_freq;
		resched_conf->force_rescheduling = 1;
		powermon_new_max_freq(max_freq);
		return EAR_SUCCESS;
	} else {

		int freq = lower_valid_freq(max_freq, num_f, f_list);
		if (freq > 0) {
			dyn_conf->max_freq = freq;
			resched_conf->force_rescheduling = 1;
			powermon_new_max_freq(freq);
			return EAR_SUCCESS;
		} else return EAR_ERROR;
	}
}

int dynconf_def_freq(uint p_id, ulong def) {
	verbose(VCONF, "Setting default freq for pid %u to %lu", p_id, def);
	if (is_valid_freq(def, num_f, f_list)) {
		if (dyn_conf->policy == p_id) {
			dyn_conf->def_freq = def;
			dyn_conf->def_p_state = frequency_freq_to_pstate(dyn_conf->def_freq);
			resched_conf->force_rescheduling = 1;
		}
		powermon_new_def_freq(p_id, def);
		return EAR_SUCCESS;
	} else {
		int freq = lower_valid_freq(def, num_f, f_list);
		if (freq > 0) {
			if (dyn_conf->policy == p_id) {
				dyn_conf->def_freq = freq;
				dyn_conf->def_p_state = frequency_freq_to_pstate(dyn_conf->def_freq);
				resched_conf->force_rescheduling = 1;
			}
			powermon_new_def_freq(p_id, freq);
			return EAR_SUCCESS;
		} else return EAR_ERROR;
	}

}

int dynconf_set_freq(ulong freq) {
	verbose(VCONF, "Setting freq to %lu", freq);
	if (is_valid_freq(freq, num_f, f_list)) {
		dyn_conf->max_freq = freq;
		dyn_conf->def_freq = freq;
		dyn_conf->def_p_state = frequency_freq_to_pstate(dyn_conf->def_freq);
		resched_conf->force_rescheduling = 1;
		powermon_set_freq(freq);
		return EAR_SUCCESS;
	} else {
		int freq2 = lower_valid_freq(freq, num_f, f_list);
		if (freq2 > 0) {
			dyn_conf->max_freq = freq2;
			dyn_conf->def_freq = freq2;
			dyn_conf->def_p_state = frequency_freq_to_pstate(dyn_conf->def_freq);
			resched_conf->force_rescheduling = 1;
			powermon_set_freq(freq2);
			return EAR_SUCCESS;
		} else return EAR_ERROR;
	}


}

int dyncon_restore_conf() {
	int pid;
	policy_conf_t *my_policy;
	/* We copy the original configuration */
	copy_my_node_conf(my_node_conf,&my_original_node_conf);
	pid=dyn_conf->policy;
	my_policy=get_my_policy_conf(my_node_conf,pid);
	dyn_conf->max_freq=frequency_pstate_to_freq(my_node_conf->max_pstate);
	dyn_conf->def_freq=frequency_pstate_to_freq(my_policy->p_state);
	dyn_conf->def_p_state=my_policy->p_state;
    memcpy(dyn_conf->settings, my_policy->settings, sizeof(double)*MAX_POLICY_SETTINGS);
	
	resched_conf->force_rescheduling=1;
	
	return EAR_SUCCESS;
}

int dynconf_red_pstates(uint p_states) {
	// Reduces p_states both the maximum and the default
	ulong i;
	uint def_pstate, max_pstate;
	ulong new_def_freq, new_max_freq;
	def_pstate = frequency_freq_to_pstate(dyn_conf->def_freq);
	max_pstate = frequency_freq_to_pstate(dyn_conf->max_freq);
	/* Reducing means incresing in the vector of pstates */
	def_pstate = def_pstate + p_states;
	max_pstate = max_pstate + p_states;

	new_def_freq = frequency_pstate_to_freq(def_pstate);
	new_max_freq = frequency_pstate_to_freq(max_pstate);


	/* reducing the maximum freq in N p_states */
	dyn_conf->max_freq = new_max_freq;
	dyn_conf->def_freq = new_def_freq;
	resched_conf->force_rescheduling = 1;

	/* We must update my_node_info */

	for (i = 0; i < my_node_conf->num_policies; i++) {
		my_node_conf->policies[i].p_state = my_node_conf->policies[i].p_state + p_states;
	}
	powermon_new_max_freq(new_max_freq);
	return EAR_SUCCESS;
}

int dynconf_set_th(ulong p_id, ulong th) {
	double dth;
	int s;
	dth=(double)th/100.0;
	if ((dth > 0 ) && (dth <=1.0 )){
		if (dyn_conf->policy==p_id){
            //currently the first setting is changed, we could pass the setting id by parameter later on
			dyn_conf->settings[0]=dth;
			resched_conf->force_rescheduling=1;	
		}
		powermon_set_th(p_id, dth);
		s = EAR_SUCCESS;
	} else s = EAR_ERROR;
	return s;
}

/* This function will propagate the status command and will return the list of node failures */
void dyncon_get_status(int fd, request_t *command) {
	status_t *status;
	char *serial_status;
	unsigned int serial_status_size;
	int num_status = propagate_status(command, my_cluster_conf.eard.port, &status);
	unsigned long return_status = num_status;
	if (num_status < 1) {
		error("Panic propagate_status returns less than 1 status");
		return_status = 0;
		write(fd, &return_status, sizeof(return_status));
		return;
	}
	powermon_get_status(&status[num_status - 1]);
	write(fd, &return_status, sizeof(return_status));
	write(fd, status, sizeof(status_t) * num_status);
	verbose(VRAPI, "Returning from dyncon_get_status\n");
	free(status);
}


void process_remote_requests(int clientfd) {
	request_t command;
	uint req;
	long ack = EAR_SUCCESS;
	verbose(VRAPI, "connection received\n");
	req = read_command(clientfd, &command);
	/* New job and end job are different */
	if (req != EAR_RC_NEW_JOB && req != EAR_RC_END_JOB) {
		if (req != EAR_RC_STATUS && req == last_command && command.time_code == last_command_time) {
			verbose(VRAPI + 1, "Recieved repeating command: %u", req);
			ack = EAR_IGNORE;
			send_answer(clientfd, &ack);
			if ((command.node_dist > 0) && (command.node_dist != last_dist)) {
				last_dist = command.node_dist;
				propagate_req(&command, my_cluster_conf.eard.port);
			}
			return;
		}
	}
	last_dist = command.node_dist;
	last_command = req;
	last_command_time = command.time_code;

	switch (req) {
		case EAR_RC_NEW_JOB:
			verbose(VRAPI, "*******************************************\n");
			verbose(VRAPI, "new_job command received %lu\n", command.my_req.new_job.job.id);
			powermon_new_job(&my_eh_rapi, &command.my_req.new_job, 0);
			break;
		case EAR_RC_END_JOB:
			powermon_end_job(&my_eh_rapi, command.my_req.end_job.jid, command.my_req.end_job.sid);
			verbose(VRAPI, "end_job command received %lu\n", command.my_req.end_job.jid);
			verbose(VRAPI, "*******************************************\n");
			break;
		case EAR_RC_MAX_FREQ:
			verbose(VRAPI, "max_freq command received %lu\n", command.my_req.ear_conf.max_freq);
			ack = dynconf_max_freq(command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_NEW_TH:
			verbose(VRAPI, "new_th command received %lu\n", command.my_req.ear_conf.th);
			ack = dynconf_set_th(command.my_req.ear_conf.p_id, command.my_req.ear_conf.th);
			break;
		case EAR_RC_INC_TH:
			verbose(VRAPI, "inc_th command received, %lu\n", command.my_req.ear_conf.th);
			ack = dynconf_inc_th(command.my_req.ear_conf.p_id, command.my_req.ear_conf.th);
			break;
		case EAR_RC_RED_PSTATE:
			verbose(VRAPI, "red_max_and_def_p_state command received\n");
			ack = dynconf_red_pstates(command.my_req.ear_conf.p_states);
			break;
		case EAR_RC_SET_FREQ:
			verbose(VRAPI, "set freq command received\n");
			ack = dynconf_set_freq(command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_DEF_FREQ:
			verbose(VRAPI, "set def freq command received\n");
			ack = dynconf_def_freq(command.my_req.ear_conf.p_id, command.my_req.ear_conf.max_freq);
			break;
		case EAR_RC_REST_CONF:
			verbose(VRAPI, "restore conf command received\n");
			ack = dyncon_restore_conf();
			break;
		case EAR_RC_PING:
			verbose(VRAPI + 1, "ping received\n");
			break;
		case EAR_RC_STATUS:
			verbose(VRAPI + 1, "Status received\n");
			dyncon_get_status(clientfd, &command);
			return;
			break;
		default:
			error("Invalid remote command\n");
			req = NO_COMMAND;
	}
	send_answer(clientfd, &ack);
#if USE_NEW_PROP
	if (req != EAR_RC_PING && req != NO_COMMAND && req != EAR_RC_NEW_JOB && req != EAR_RC_END_JOB)
#else
	if (command.node_dist > 0 && req != EAR_RC_PING && req != NO_COMMAND)
#endif
	{
		verbose(VRAPI + 1, "command=%d propagated distance=%d", req, command.node_dist);
		propagate_req(&command, my_cluster_conf.eard.port);
	}
}

/*
*	THREAD to process remote queries
*/

void *eard_dynamic_configuration(void *tmp)
{
	my_tmp = (char *) tmp;

	verbose(VRAPI, "RemoteAPI thread created\n");

	DC_set_sigusr1();

	if (pthread_setname_np(pthread_self(), TH_NAME)) {
		error("Setting name for %s thread %s", TH_NAME, strerror(errno));
	}

	if (energy_init(NULL, &my_eh_rapi) != EAR_SUCCESS) {
		error("Error initializing energy node in %s thread", TH_NAME);
	}

	num_f = frequency_get_num_pstates();
	f_list = frequency_get_freq_rank_list();
	print_f_list(num_f, f_list);
	verbose(VRAPI + 2, "We have %u valid p_states\n", num_f);
#if USE_NEW_PROP
	verbose(0, "Init for ips\n");
	init_ips(&my_cluster_conf);
#endif


	verbose(VRAPI, "Creating socket for remote commands\n");
	eards_remote_socket = create_server_socket(my_cluster_conf.eard.port);
	if (eards_remote_socket < 0) {
		error("Error creating socket, exiting eard_dynamic_configuration thread\n");
		pthread_exit(0);
	}
	/*
	*	MAIN LOOP
	*/
	do {
		verbose(VRAPI + 1, "waiting for remote commands\n");
		eards_client = wait_for_client(eards_remote_socket, &eards_remote_client);
		if (eards_client < 0) {
			error(" wait_for_client returns error\n");
			close_server_socket(eards_remote_socket);
			eards_remote_socket = create_server_socket(my_cluster_conf.eard.port);
			if (eards_remote_socket < 0) {
				eard_must_exit = 1;
				error("Panic, we cannot create socket for connection again,exiting");
			}
		} else {
			process_remote_requests(eards_client);
			close(eards_client);
		}
	} while (eard_must_exit == 0);
	warning("eard_dynamic_configuration exiting\n");
#if USE_NEW_PROP
	close_ips();
#endif
	//ear_conf_shared_area_dispose(my_tmp);
	close_server_socket(eards_remote_socket);
	pthread_exit(0);

}

