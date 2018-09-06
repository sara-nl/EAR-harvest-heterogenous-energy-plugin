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
#include <unistd.h>

#include <common/config.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/eard_rapi.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/types/job.h>

// Verbosity
static const char *__NAME__ = "eard_rapi";

static int eards_remote_connected=0;
static int eards_sfd=-1;

//#define API_DEBUG 1

// Sends a command to eard
int send_command(request_t *command)
{
	ulong ack;
	int ret;
	VERBOSE_N(2,"Sending command %u\n",command->req);
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

// based on getaddrinfo  man page
int eards_remote_connect(char *nodename,uint port)
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

	sprintf(port_number,"%d",port);
   	s = getaddrinfo(nodename, port_number, &hints, &result);
    if (s != 0) {
		#if API_DEBUG
		fprintf(stderr,"getaddrinfo failt for %s and %s\n",nodename,port_number);
		#endif
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
	command.req=EAR_RC_NEW_JOB;
	copy_application(&command.my_req.new_job,new_job);
	VERBOSE_N(2,"command %u job_id %d,%d\n",command.req,command.my_req.new_job.job.id,command.my_req.new_job.job.step_id);
	return send_command(&command);
}

int eards_end_job(job_id jid,job_id sid)
{
    request_t command;
    command.req=EAR_RC_END_JOB;
	command.my_req.end_job.jid=jid;
	command.my_req.end_job.sid=sid;
	VERBOSE_N(2,"command %u job_id %d step_id %d\n",command.req,command.my_req.end_job.jid,command.my_req.end_job.sid);
	return send_command(&command);
}

int eards_set_max_freq(unsigned long freq)
{
	request_t command;
	command.req=EAR_RC_MAX_FREQ;
    command.my_req.ear_conf.max_freq=freq;
	return send_command(&command);
}

int eards_set_freq(unsigned long freq)
{
    request_t command;
    command.req=EAR_RC_SET_FREQ;
    command.my_req.ear_conf.max_freq=freq;
    return send_command(&command);
}
int eards_set_def_freq(unsigned long freq)
{
    request_t command;
    command.req=EAR_RC_DEF_FREQ;
    command.my_req.ear_conf.max_freq=freq;
    return send_command(&command);
}


int eards_red_max_and_def_freq(uint p_states)
{
    request_t command;
    command.req=EAR_RC_RED_PSTATE;
    command.my_req.ear_conf.p_states=p_states;
    return send_command(&command);
}

int eards_restore_conf()
{
    request_t command;
    command.req=EAR_RC_REST_CONF;
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

// New th must be passed as % th=0.05 --> 5
int eards_inc_th(unsigned long th)
{
    request_t command;
    command.req=EAR_RC_INC_TH;
    command.my_req.ear_conf.th=th;
    return send_command(&command);
}
int eards_ping()
{
    request_t command;
    command.req=EAR_RC_PING;
    command.node_dist = 0;
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
void increase_th_all_nodes(ulong th, cluster_conf_t my_cluster_conf)
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
	    		    VERBOSE_N(0,"Error connecting with node %s", node_name);
            	}else{
	        		VERBOSE_N(1,"Increasing the PerformanceEfficiencyGain in node %s by %lu\n", node_name,th);
		        	if (!eards_inc_th(th)) VERBOSE_N(0,"Error increasing the th for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}

void red_max_freq_all_nodes(ulong ps, cluster_conf_t my_cluster_conf)
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
	    		    VERBOSE_N(0,"Error connecting with node %s", node_name);
            	}else{
    
                	VERBOSE_N(1,"Reducing  the frequency in node %s by %lu\n", node_name,ps);
		        	if (!eards_red_max_and_def_freq(ps)) VERBOSE_N(0,"Error reducing the max freq for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}

void ping_all_nodes(cluster_conf_t my_cluster_conf)
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
                    VERBOSE_N(0,"Error connecting with node %s", node_name);
                }else{

                    VERBOSE_N(1,"Node %s ping!\n", node_name);
                    if (!eards_ping()) VERBOSE_N(0,"Error doing ping for node %s", node_name);
                    eards_remote_disconnect();
                }
            }
        }
    }
}

void new_ping_all_nodes2(cluster_conf_t my_cluster_conf)
{
    char *node_name = "r22u21";
    int rc=eards_remote_connect(node_name, my_cluster_conf.eard.port);
    if (rc<0){
        VERBOSE_N(0,"Error connecting with node %s", node_name);
    }else{
        VERBOSE_N(1,"Node %s ping!\n", node_name);
        request_t command;
        command.req=EAR_RC_PING;
        command.node_dist = 2;
        if (!send_command(&command)) VERBOSE_N(0,"Error doing ping for node %s", node_name);
        eards_remote_disconnect();
    }

}

void new_ping_all_nodes(cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc; 
    char node_name[256];
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {   
            request_t command;
            command.req = EAR_RC_PING;
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

                command.node_dist = (my_cluster_conf.islands[i].ranges[j].end - my_cluster_conf.islands[i].ranges[j].start)/4;
                int t = 1;
                while (t < command.node_dist) t *=2;
                command.node_dist = t;
            }   
                rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                if (rc<0){
                    VERBOSE_N(0,"Error connecting with node %s", node_name);
                }else{
                    VERBOSE_N(1,"Node %s with distande %d ping!\n", node_name, command.node_dist);
                    if (!send_command(&command)) VERBOSE_N(0,"Error doing ping for node %s", node_name);
                    eards_remote_disconnect();
                }
            }
        }
}


void red_def_freq_all_nodes(ulong ps, cluster_conf_t my_cluster_conf)
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
	    		    VERBOSE_N(0,"Error connecting with node %s", node_name);
            	}else{
                	VERBOSE_N(1,"Reducing  the default and maximumfrequency in node %s by %lu\n", node_name,ps);
		        	if (!eards_red_max_and_def_freq(ps)) VERBOSE_N(0,"Error reducing the default freq for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}



void reduce_frequencies_all_nodes(ulong freq, cluster_conf_t my_cluster_conf)
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
                    VERBOSE_N(0,"Error connecting with node %s",node_name);
                }else{
                	VERBOSE_N(1,"Setting  the frequency in node %s to %lu\n", node_name, freq);
                	if (!eards_set_freq(freq)) VERBOSE_N(0,"Error reducing the freq for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }
}

void set_def_freq_all_nodes(ulong freq, cluster_conf_t my_cluster_conf)
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
                    VERBOSE_N(0,"Error connecting with node %s",node_name);
                }else{
                	VERBOSE_N(1,"Setting  the frequency in node %s to %lu\n", node_name, freq);
                	if (!eards_set_def_freq(freq)) VERBOSE_N(0,"Error setting the freq for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }
}

void restore_conf_all_nodes(cluster_conf_t my_cluster_conf)
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
                    VERBOSE_N(0,"Error connecting with node %s",node_name);
                }else{
                	VERBOSE_N(1,"Restoring the configuartion in node %s\n", node_name);
                	if (!eards_restore_conf()) VERBOSE_N(0,"Error restoring the configuration for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }

}
