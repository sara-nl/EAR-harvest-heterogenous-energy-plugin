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
#include <arpa/inet.h>
#include <netdb.h> 
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include <common/config.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/eard_rapi.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/types/job.h>

// Verbosity
static const char *__NAME__ = "eard";
static const char *__HOST__ ="api";

static int eards_remote_connected=0;
static int eards_sfd=-1;

//#define API_DEBUG 1

// Sends a command to eard
int send_command(request_t *command)
{
	ulong ack;
	int ret;
	eard_verbose(2,"Sending command %u\n",command->req);
	if ((ret=write(eards_sfd,command,sizeof(request_t)))!=sizeof(request_t)){
		if (ret<0){ 
			eard_verbose(0,"Error sending command %s\n",strerror(errno));
		}else{ 
			eard_verbose(0,"Error sending command \n");
		}
	}
	ret=read(eards_sfd,&ack,sizeof(ulong));
	if (ret<0){
		eard_verbose(0,"Error receiving ack %s\n",strerror(errno));
	}
	else if (ret!=sizeof(ulong)){
		eard_verbose(0,"Error receiving ack %d\n",ret);
	}
	return (ret==sizeof(ulong)); // Should we return ack ?
}

//specifically sends and reads the ack of a status command
int send_status(request_t *command, status_t **status)
{
	ulong ack;
	int ret;
	int total, pending;
    status_t *return_status;
	eard_verbose(1,"Sending command %u\n",command->req);
	if ((ret=write(eards_sfd,command,sizeof(request_t)))!=sizeof(request_t)){
		if (ret<0){ 
			eard_verbose(0,"Error sending command (status) %s\n",strerror(errno));
		}else{ 
			eard_verbose(0,"Error sending command (status) ret=%d expected=%d\n",ret,sizeof(request_t));
		}
	}
	eard_verbose(1,"Reading ack size \n");
	/* We assume first long will not block */
	ret=read(eards_sfd,&ack,sizeof(ulong));
	if (ret<0){
		eard_verbose(0,"Error receiving ack in (status) (%s) \n",strerror(errno));
        return EAR_ERROR;
	}
    if (ack < 1){
        eard_verbose(0, "Number of status expected is not valid: %d", ack);
        return EAR_ERROR;
    }
	eard_verbose(0,"Waiting for %d ack bytes\n",ack);
    return_status = calloc(ack, sizeof(status_t));
	if (return_status==NULL){
		eard_verbose(0, "Not enough memory at send_status");
		return EAR_ERROR;
	}
	total=0;
	pending=sizeof(status_t)*ack;
    ret = read(eards_sfd, (char *)return_status+total, pending);
	if (ret<0){
		eard_verbose(0, "Error by reading status (%s)",strerror(errno));
        free(return_status);
		return EAR_ERROR;
	}
	total+=ret;
	pending-=ret;
	while ((ret>0) && (pending>0)){
    	ret = read(eards_sfd, (char *)return_status+total, pending);
		if (ret<0){
			eard_verbose(0, "Error by reading status (%s)",strerror(errno));
        	free(return_status);
			return EAR_ERROR;
		}
		total+=ret;
		pending-=ret;
	}
    *status = return_status;
	eard_verbose(1,"Returning from send_status with %d\n",ack);
	return ack;
}

int set_socket_block(int sfd, char blocking)
{
    if (sfd < 0) return EAR_ERROR;

    int flags = fcntl(sfd, F_GETFL, 0);
    //if flags is -1 fcntl returned an error
    if (flags == -1) return EAR_ERROR;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(sfd, F_SETFL, flags) != 0) return EAR_ERROR;
    return EAR_ERROR;
}

// based on getaddrinfo  man page
int eards_remote_connect(char *nodename,uint port)
{
    int client_sock;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
	char port_number[50]; 	// that size needs to be validated
    int sfd, s, j, res;
    fd_set set;

	if (eards_remote_connected){ 
		eard_verbose(1,"Connection already done!\n");
		return eards_sfd;
	}
   	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

	sprintf(port_number,"%d",port);
   	s = getaddrinfo(nodename, port_number, &hints, &result);
    if (s != 0) {
		#if API_DEBUG
		fprintf(stderr,"getaddrinfo failt for %s and %s\n",nodename,port_number);
		#endif
		return EAR_ERROR;
    }

    struct timeval timeout;
    memset(&timeout, 0, sizeof(struct timeval));
    timeout.tv_sec = 0;
    timeout.tv_usec = 5000;
    int optlen, valopt;

   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1) // if we can not create the socket we continue
            continue;

        set_socket_block(sfd, 0);
        
        int res = connect(sfd, rp->ai_addr, rp->ai_addrlen);
        if (res < 0 && errno != EINPROGRESS)
        {
            close(sfd);
            continue;
        }
        if (res == 0) //sucessful connection
        {
            set_socket_block(sfd, 1);
            break;
        }
        else
        {
            FD_ZERO(&set);
            FD_SET(sfd, &set);
            if (select(sfd+1, NULL, &set, NULL, &timeout) > 0) 
            {
                optlen = sizeof(int);
                getsockopt(sfd, SOL_SOCKET, SO_ERROR, (void *)(&valopt), &optlen);
                if (valopt)
                {
                    fprintf(stderr, "Error opening connection %s",nodename);
                    close(sfd);
                    continue;
                }
            }
            else
            {
                fprintf(stderr, "Timeout connecting to %s node", nodename);
                close(sfd);
                continue;
            }
            set_socket_block(sfd, 1);
            break;
        }
    }

   	if (rp == NULL) {               /* No address succeeded */
		#if API_DEBUG
		fprintf(stderr,"Failing in connecting to remote erds\n");
		#endif
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
	command.node_dist = 0;

	command.req=EAR_RC_NEW_JOB;
    command.node_dist = 0;
    command.time_code = time(NULL);
	copy_application(&command.my_req.new_job,new_job);
	eard_verbose(2,"command %u job_id %d,%d\n",command.req,command.my_req.new_job.job.id,command.my_req.new_job.job.step_id);
	return send_command(&command);
}

int eards_end_job(job_id jid,job_id sid)
{
    request_t command;
	command.node_dist = 0;

    command.req=EAR_RC_END_JOB;
    command.node_dist = 0;
    command.time_code = time(NULL);
	command.my_req.end_job.jid=jid;
	command.my_req.end_job.sid=sid;
//	command.my_req.end_job.status=status;
	//eard_verbose(2,"command %u job_id %d step_id %d status %d\n",command.req,command.my_req.end_job.jid,command.my_req.end_job.sid,command.my_req.end_job.status);
	eard_verbose(2,"command %u job_id %d step_id %d \n",command.req,command.my_req.end_job.jid,command.my_req.end_job.sid);
	return send_command(&command);
}

int eards_set_max_freq(unsigned long freq)
{
	request_t command;
	command.node_dist = 0;
	command.req=EAR_RC_MAX_FREQ;
    command.node_dist = 0;
    command.time_code = time(NULL);
    command.my_req.ear_conf.max_freq=freq;
	return send_command(&command);
}

int eards_set_freq(unsigned long freq)
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_SET_FREQ;
    command.node_dist = 0;
    command.time_code = time(NULL);
    command.my_req.ear_conf.max_freq=freq;
    return send_command(&command);
}
int eards_set_def_freq(unsigned long freq)
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_DEF_FREQ;
    command.node_dist = 0;
    command.time_code = time(NULL);
    command.my_req.ear_conf.max_freq=freq;
    return send_command(&command);
}


int eards_red_max_and_def_freq(uint p_states)
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_RED_PSTATE;
    command.node_dist = 0;
    command.time_code = time(NULL);
    command.my_req.ear_conf.p_states=p_states;
    return send_command(&command);
}

int eards_restore_conf()
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_REST_CONF;
    command.node_dist = 0;
    command.time_code = time(NULL);
    return send_command(&command);
}



// New th must be passed as % th=0.75 --> 75
int eards_set_th(unsigned long th)
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_NEW_TH;
    command.node_dist = 0;
    command.time_code = time(NULL);
    command.my_req.ear_conf.th=th;
    return send_command(&command);
}

// New th must be passed as % th=0.05 --> 5
int eards_inc_th(unsigned long th)
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_INC_TH;
    command.node_dist = 0;
    command.time_code = time(NULL);
    command.my_req.ear_conf.th=th;
    return send_command(&command);
}
int eards_ping()
{
    request_t command;
	command.node_dist = 0;
    command.req=EAR_RC_PING;
    command.node_dist = 0;
    command.time_code = time(NULL);
    return send_command(&command);
}

int eards_remote_disconnect()
{
	eards_remote_connected=0;
	close(eards_sfd);
	return EAR_SUCCESS;
}


/*
*	SAME FUNCTIONALLITY BUT SENT TO ALL NODES
*/
void old_increase_th_all_nodes(ulong th, cluster_conf_t my_cluster_conf)
{
	int i, j, k, rc;
    char node_name[256];

    for (i=0;i < my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {
                if (k == -1)
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10)
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                }
    	        rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
        	    if (rc<0){
	    		    eard_verbose(0,"Error connecting with node %s", node_name);
            	}else{
	        		eard_verbose(1,"Increasing the PerformanceEfficiencyGain in node %s by %lu\n", node_name,th);
		        	if (!eards_inc_th(th)) eard_verbose(0,"Error increasing the th for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}

void old_red_max_freq_all_nodes(ulong ps, cluster_conf_t my_cluster_conf)
{
	int i, j, k, rc;
    char node_name[256];
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {
                if (k == -1)
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10)
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                }
    	        rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
        	    if (rc<0){
	    		    eard_verbose(0,"Error connecting with node %s", node_name);
            	}else{
    
                	eard_verbose(1,"Reducing  the frequency in node %s by %lu\n", node_name,ps);
		        	if (!eards_red_max_and_def_freq(ps)) eard_verbose(0,"Error reducing the max freq for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}

void old_ping_all_nodes(cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc; 
    char node_name[256];
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {   
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {   
                if (k == -1) 
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10) 
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                }   
                rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                if (rc<0){
                    eard_verbose(0,"Error connecting with node %s", node_name);
                }else{

                    eard_verbose(1,"Node %s ping!\n", node_name);
                    if (!eards_ping()) eard_verbose(0,"Error doing ping for node %s", node_name);
                    eards_remote_disconnect();
                }
            }
        }
    }
}

void increase_th_all_nodes(ulong th, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_INC_TH;
    command.my_req.ear_conf.th=th;
    send_command_all(command, my_cluster_conf);
}

void set_th_all_nodes(ulong th, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_NEW_TH;
    command.my_req.ear_conf.th=th;
    send_command_all(command, my_cluster_conf);
}

void ping_all_nodes(cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req = EAR_RC_PING;
    send_command_all(command, my_cluster_conf);
}

void set_max_freq_all_nodes(ulong max_freq, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_MAX_FREQ;
    command.my_req.ear_conf.max_freq = max_freq;
    send_command_all(command, my_cluster_conf);
}

void set_freq_all_nodes(ulong freq, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_SET_FREQ;
    command.my_req.ear_conf.max_freq = freq;
    send_command_all(command, my_cluster_conf);
}

void red_def_max_pstate_all_nodes(ulong pstate, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_RED_PSTATE;
    command.my_req.ear_conf.p_states = pstate;
    send_command_all(command, my_cluster_conf);
}

void reduce_frequencies_all_nodes(ulong freq, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_DEF_FREQ;
    command.my_req.ear_conf.max_freq=freq;
    send_command_all(command, my_cluster_conf);
}

void restore_conf_all_nodes(cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_REST_CONF;
    send_command_all(command, my_cluster_conf);
}

void set_def_freq_all_nodes(ulong freq, ulong policy, cluster_conf_t my_cluster_conf)
{
    request_t command;
    command.req=EAR_RC_DEF_FREQ;
    command.my_req.ear_conf.max_freq = freq;
	command.my_req.ear_conf.p_id = policy;
    send_command_all(command, my_cluster_conf);
}

int correct_status(uint target_ip, request_t *command, uint port, status_t **status)
{
    status_t *final_status, *status1 = NULL, *status2 = NULL;
    int total_status, num_status1 = 0, num_status2 = 0;
	eard_verbose(1,"correct_status for ip %d with distance %d\n",target_ip,command->node_dist);
    if (command->node_dist < 1) {
        final_status = calloc(1, sizeof(status_t));
        final_status[0].ip = target_ip;
        final_status[0].ok = STATUS_BAD;
        *status = final_status;
        return 1;
    }

    char nextip1[50], nextip2[50];

    struct sockaddr_in temp;
    unsigned int self_ip, ip1, ip2; 
    self_ip = ip1 = ip2 = htonl(target_ip);
    ip1 += command->node_dist;
    temp.sin_addr.s_addr = ntohl(ip1);

    strcpy(nextip1, inet_ntoa(temp.sin_addr));

    ip2 -= command->node_dist;
    temp.sin_addr.s_addr = ntohl(ip2);
    strcpy(nextip2, inet_ntoa(temp.sin_addr));

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
		eard_verbose(1,"connection ok, sending status requests %s\n",nextip1);
        if ((num_status1 = send_status(command, &status1)) < 1)
        {
            fprintf(stderr, "Error propagating command to node %s\n", nextip1);
            eards_remote_disconnect();
            num_status1 = correct_status(ntohl(ip1), command, port, &status1);
        }
        else eards_remote_disconnect();
    }

	eard_verbose(1,"Correcting second node\n");

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
		eard_verbose(1,"connection ok, sending status requests %s\n",nextip2);
        if ((num_status2 = send_status(command, &status2)) < 1)
        {
            fprintf(stderr, "Error propagating command to node %s\n", nextip2);
            eards_remote_disconnect();
            num_status2 = correct_status(ntohl(ip2), command, port, &status2);
        }
        else eards_remote_disconnect();
    } 

    total_status = num_status1 + num_status2;
    final_status = calloc(total_status + 1, sizeof(status_t));
    memcpy(final_status, status1, sizeof(status_t)*num_status1);
    memcpy(&final_status[num_status1], status2, sizeof(status_t)*num_status2);
    final_status[total_status].ip = ntohl(self_ip);
    final_status[total_status].ok = STATUS_BAD;
    *status = final_status;
    free(status1);
    free(status2);
	eard_verbose(1,"correct_status ends return value=%d\n",total_status + 1);
    return total_status + 1;
}

void correct_error(uint target_ip, request_t *command, uint port)
{
    if (command->node_dist < 1) return;
    char nextip1[50], nextip2[50];

    struct sockaddr_in temp;
    unsigned int ip1, ip2; 
    ip1 = ip2 = htonl(target_ip);
    ip1 += command->node_dist;
    temp.sin_addr.s_addr = ntohl(ip1);

    strcpy(nextip1, inet_ntoa(temp.sin_addr));

    ip2 -= command->node_dist;
    temp.sin_addr.s_addr = ntohl(ip2);
    strcpy(nextip2, inet_ntoa(temp.sin_addr));

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

int correct_status_starter(char *host_name, request_t *command, uint port, status_t **status)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    int ip1, ip2;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    int host_ip = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

   	s = getaddrinfo(host_name, NULL, &hints, &result);
    if (s != 0) {
		fprintf(stderr,"getaddrinfo fails for host %s (%s)\n",host_name,strerror(errno));
		return EAR_ERROR;
    }

   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in*) (rp->ai_addr);
            host_ip = saddr->sin_addr.s_addr;
        }
    }
    freeaddrinfo(result);
    return correct_status(host_ip, command, port, status);
}


void correct_error_starter(char *host_name, request_t *command, uint port)
{
	if (command->node_dist < 1) return;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    int ip1, ip2;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    int host_ip = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

   	s = getaddrinfo(host_name, NULL, &hints, &result);
    if (s != 0) {
		fprintf(stderr,"getaddrinfo fails for host %s (%s)\n",host_name,strerror(errno));
		return;
    }

   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in*) (rp->ai_addr);
            host_ip = saddr->sin_addr.s_addr;
        }
    }
    freeaddrinfo(result);
    correct_error(host_ip, command, port);
}

void send_command_all(request_t command, cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc; 
    char node_name[256];
    time_t ctime = time(NULL);
    command.time_code = ctime;
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {   
            k = my_cluster_conf.islands[i].ranges[j].start;
            command.node_dist = 0;
            if (k == -1) 
                sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
            else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
            else {
                k += (my_cluster_conf.islands[i].ranges[j].end - my_cluster_conf.islands[i].ranges[j].start)/2;
                if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10) 
                    sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                else 
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 

                command.node_dist = (my_cluster_conf.islands[i].ranges[j].end - my_cluster_conf.islands[i].ranges[j].start)/2;
                int t = 1;
                while (t < command.node_dist) t *= 2;
                command.node_dist = t;
            }   
            rc=eards_remote_connect(node_name, my_cluster_conf.eard.port);
            if (rc<0){
                eard_verbose(0,"Error connecting with node %s, trying to correct it", node_name);
                correct_error_starter(node_name, &command, my_cluster_conf.eard.port);
            }
            else{
                eard_verbose(1,"Node %s with distance %d contacted!\n", node_name, command.node_dist);
                if (!send_command(&command)) {
                    eard_verbose(0,"Error sending command to node %s, trying to correct it", node_name);
                    correct_error_starter(node_name, &command, my_cluster_conf.eard.port);
                }
                eards_remote_disconnect();
            }
        }
    }
}

int status_all_nodes(cluster_conf_t my_cluster_conf, status_t **status)
{
    int i, j, k, rc; 
    char node_name[256];
    status_t *temp_status, *all_status = NULL;
    int num_all_status = 0, num_temp_status;
    request_t command;
    time_t ctime = time(NULL);
    command.time_code = ctime;
    command.req = EAR_RC_STATUS;
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {   
            num_temp_status = 0;
            k = my_cluster_conf.islands[i].ranges[j].start;
            command.node_dist = 0;
            if (k == -1) 
                sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
            else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
            else {
                k += (my_cluster_conf.islands[i].ranges[j].end - my_cluster_conf.islands[i].ranges[j].start)/2;
                if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10) 
                    sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                else 
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 

                command.node_dist = (my_cluster_conf.islands[i].ranges[j].end - my_cluster_conf.islands[i].ranges[j].start)/2 + 1;
                int t = 1;
                while (t < command.node_dist) t *= 2;
                command.node_dist = t;
            }   

            rc=eards_remote_connect(node_name, my_cluster_conf.eard.port);
            if (rc<0){
                eard_verbose(0,"Error connecting with node %s, trying to correct it", node_name);
                num_temp_status = correct_status_starter(node_name, &command, my_cluster_conf.eard.port, &temp_status);
            }
            else{
                eard_verbose(1,"Node %s with distance %d contacted with status!", node_name, command.node_dist);
                if ((num_temp_status = send_status(&command, &temp_status)) < 1) {
                    eard_verbose(0,"Error doing status for node %s, trying to correct it", node_name);
                    num_temp_status = correct_status_starter(node_name, &command, my_cluster_conf.eard.port, &temp_status);
                }
                eards_remote_disconnect();
            }
            if (num_temp_status > 0)
            {
                all_status = realloc(all_status, sizeof(status_t)*(num_all_status+num_temp_status));
                memcpy(&all_status[num_all_status], temp_status, sizeof(status_t)*num_temp_status);
                free(temp_status);
                num_all_status += num_temp_status;
            }

        }
    }
    *status = all_status;
    return num_all_status;
}

void old_red_def_freq_all_nodes(ulong ps, cluster_conf_t my_cluster_conf)
{
	int i, j, k, rc;
    char node_name[256];
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {
                if (k == -1)
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10)
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                }
    	        rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
        	    if (rc<0){
	    		    eard_verbose(0,"Error connecting with node %s", node_name);
            	}else{
                	eard_verbose(1,"Reducing  the default and maximumfrequency in node %s by %lu\n", node_name,ps);
		        	if (!eards_red_max_and_def_freq(ps)) eard_verbose(0,"Error reducing the default freq for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}



void old_reduce_frequencies_all_nodes(ulong freq, cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc;
    char node_name[256];

    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {
                if (k == -1)
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10)
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    
                }

                rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                if (rc<0){
                    eard_verbose(0,"Error connecting with node %s",node_name);
                }else{
                	eard_verbose(1,"Setting  the frequency in node %s to %lu\n", node_name, freq);
                	if (!eards_set_freq(freq)) eard_verbose(0,"Error reducing the freq for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }
}

void old_set_def_freq_all_nodes(ulong freq, cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc;
    char node_name[256];

    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {
                if (k == -1)
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10)
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    
                }

                rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                if (rc<0){
                    eard_verbose(0,"Error connecting with node %s",node_name);
                }else{
                	eard_verbose(1,"Setting  the frequency in node %s to %lu\n", node_name, freq);
                	if (!eards_set_def_freq(freq)) eard_verbose(0,"Error setting the freq for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }
}

void old_restore_conf_all_nodes(cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc;
    char node_name[256];

    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {
                if (k == -1)
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10)
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k);
                    
                }

                rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                if (rc<0){
                    eard_verbose(0,"Error connecting with node %s",node_name);
                }else{
                	eard_verbose(1,"Restoring the configuartion in node %s\n", node_name);
                	if (!eards_restore_conf()) eard_verbose(0,"Error restoring the configuration for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }

}
