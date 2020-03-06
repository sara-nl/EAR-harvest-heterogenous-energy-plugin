/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * * 
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * * 
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING  
 * */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <common/config.h>
//#define SHOW_DEBUGS 0
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/system/execute.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/power_monitor.h>
#include <daemon/powercap.h>


/* This version is a prototype and should be replaced by an INM plugin+GPU commands for powercap */

#define INM_ENABLE_CMD	"ipmitool raw 0x3a 0xc7 0x01"
#define INM_ENABLE_POLICY_CONTROL_CMD "ipmitool -b 0 -t 0x2c raw 0x2E 0XC0 0X57 01 00 01 00 00" 
#define INM_SET_POWERCAP_POLICY_CMD "NO_CMD"
#define INM_GET_POWERCAP_POLICY_CMD "NO_CMD"
#define INM_SET_POWERCAP_VALUE_CMD "ipmitool -vvvvv -b 0 -t 0x2c raw 0x2E 0xD0 0x57 0x01 0x00 %#X %#X %#X"
#define INM_GET_POWERCAP_CMD "NO_CMD"

static node_powercap_opt_t my_pc_opt;
static int my_ip;

#define min(a,b) ((a<b)?a:b)
uint compute_power_to_release(uint current)
{
	return 0;
}

uint compute_power_to_ask(uint current)
{
	return 0;
}

uint more_power(uint current)
{
	return 0;
}
uint free_power(uint current)
{
	return 0;
}
uint ok_power(uint current)
{
	return 1;
}

int do_cmd(char *cmd)
{
	if (strcmp(cmd,"NO_CMD")) return 0;
	return 1;
}

void print_node_powercap_opt(node_powercap_opt_t *my_powercap_opt)
{
	fprintf(stderr,"cuurent %u pc_def %u pc_idle %u th_inc %u th_red %u th_release %u max_new_jobs %u\n",
	my_powercap_opt->current_pc,my_powercap_opt->def_powercap,my_powercap_opt->powercap_idle,my_powercap_opt->th_inc,my_powercap_opt->th_red, my_powercap_opt->th_release,my_powercap_opt->max_inc_new_jobs);
}

void set_default_node_powercap_opt(node_powercap_opt_t *my_powercap_opt)
{
	my_powercap_opt->def_powercap=powermon_get_powercap_def();
	my_powercap_opt->powercap_idle=powermon_get_powercap_def();
	my_powercap_opt->current_pc=powermon_get_powercap_def();
	my_powercap_opt->last_t1_allocated=powermon_get_powercap_def();
	my_powercap_opt->released=0;
	my_powercap_opt->th_inc=25;
	my_powercap_opt->th_red=25;
	my_powercap_opt->th_release=25;
	my_powercap_opt->powercap_status=PC_STATUS_ERROR;
	my_powercap_opt->max_inc_new_jobs=0;
	my_powercap_opt->requested=0;
	pthread_mutex_init(&my_powercap_opt->lock,NULL);
}

int powercap_init(node_powercap_opt_t *my_powercap_opt)
{
	char cmd[1024];

	set_default_node_powercap_opt(&my_pc_opt);
	print_node_powercap_opt(&my_pc_opt);
	my_ip=0;
	
	/* Enable INM commands */
	sprintf(cmd,INM_ENABLE_CMD);
	if (do_cmd(cmd)){
	if (execute(cmd)!=EAR_SUCCESS){ 
		debug("Error executing INM CMD enable");
		return EAR_ERROR;
	}
	}
	/* Enable powercap policy control */
	sprintf(cmd,INM_ENABLE_POLICY_CONTROL_CMD);
	if (do_cmd(cmd)){
	if (execute(cmd)!=EAR_SUCCESS){
		debug("Error executing INM CMD Policy control");
		return EAR_ERROR;
	}
	}
	/* Set powercap policy control */
	sprintf(cmd,INM_SET_POWERCAP_POLICY_CMD);
	if (do_cmd(cmd)){
	if (execute(cmd)!=EAR_SUCCESS){
		debug("Error executing INM CMD Set Policy control");
		return EAR_ERROR;
	}
	}
	/* Get powercap policy for validation */	
	sprintf(cmd,INM_GET_POWERCAP_POLICY_CMD);
	if (do_cmd(cmd)){
  if (execute(cmd)!=EAR_SUCCESS){
    debug("Error executing INM CMD Get Policy control");
    return EAR_ERROR;
  }
	}
	my_pc_opt.powercap_status=PC_STATUS_OK;
	return EAR_SUCCESS;	
}

int is_powercap_set()
{
	/* 0 means unlimited */
	return (my_pc_opt.current_pc!=0);
}

uint get_powercap_value()
{
	return my_pc_opt.current_pc;
}

int set_powercap_value(uint domain,uint limit)
{
	char cmd[1024];
	state_t ret;
	uint16_t sized_limit=(uint16_t)limit;
	char *limitv=(char *)&sized_limit;
	if (limit==my_pc_opt.current_pc) return EAR_SUCCESS;
	debug("set_powercap_value domain %u limit %u",domain,limit);
	sprintf(cmd,INM_SET_POWERCAP_VALUE_CMD,domain,(uint8_t)limitv[0],(uint8_t)limitv[1]);
	my_pc_opt.current_pc=limit;
	
	return execute(cmd);
}


int powercap_idle_to_run()
{
	uint next_pc;
	while(pthread_mutex_trylock(&my_pc_opt.lock)); /* can we create some deadlock because of status ? */
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
		debug("Goin from idle to run ");
		/* There is enough power for me */
		if ((my_pc_opt.last_t1_allocated+my_pc_opt.max_inc_new_jobs)>=my_pc_opt.def_powercap){	
			/* if we already had de power, we just set the status as OK */
			if (my_pc_opt.last_t1_allocated>=my_pc_opt.def_powercap){
				my_pc_opt.powercap_status=PC_STATUS_OK;
				my_pc_opt.released=0;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}else{
				/* We must use extra power */
				my_pc_opt.last_t1_allocated=min(my_pc_opt.def_powercap,my_pc_opt.last_t1_allocated+my_pc_opt.max_inc_new_jobs);
				my_pc_opt.powercap_status=PC_STATUS_OK;
				my_pc_opt.released=0;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}	
		}else{ /* we must ask more power */
				uint pending;
				my_pc_opt.last_t1_allocated+=my_pc_opt.max_inc_new_jobs;
				my_pc_opt.powercap_status=PC_STATUS_ASK_DEF;
				pending=my_pc_opt.def_powercap-my_pc_opt.last_t1_allocated;
				my_pc_opt.requested=pending;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
		}
		break;
    case PC_STATUS_OK:
    case PC_STATUS_GREEDY:
    case PC_STATUS_RELEASE:
    case PC_STATUS_ASK_DEF:
		error("We go to run and we were not in idle ");
		break;
	}
	pthread_mutex_unlock(&my_pc_opt.lock);
	return EAR_SUCCESS;
}

int powercap_run_to_idle()
{
	uint next_pc;
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
		error("going from run to idle and we were in idle");
    break;
    case PC_STATUS_OK:
    case PC_STATUS_GREEDY:
    case PC_STATUS_ASK_DEF:
		debug("Goin from run to idle");
		uint rel;
		my_pc_opt.released=my_pc_opt.last_t1_allocated-my_pc_opt.powercap_idle;
		my_pc_opt.requested=0;
		my_pc_opt.powercap_status=PC_STATUS_IDLE;
		set_powercap_value(DOMAIN_NODE,my_pc_opt.powercap_idle);
    break;
  }
  pthread_mutex_unlock(&my_pc_opt.lock);
  return EAR_SUCCESS;
}
int periodic_metric_info(double cp)
{
	uint current=(uint)cp;
	debug("PM event, current power %u powercap %u allocated %u status %u",
		current,my_pc_opt.current_pc,my_pc_opt.last_t1_allocated,my_pc_opt.powercap_status);
	while(pthread_mutex_trylock(&my_pc_opt.lock));
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
			debug("Idle node, no actions");
			break;
		case PC_STATUS_OK:
			/******* FREE POWER ******/
			if (free_power(current)){
				/* We are consuming few power, we can release son percentage */
				uint TBR,nextpc;
				TBR=compute_power_to_release(current);
				nextpc=my_pc_opt.current_pc-TBR; 
				my_pc_opt.released=TBR;
				my_pc_opt.requested=0;
				my_pc_opt.powercap_status=PC_STATUS_RELEASE;
				set_powercap_value(DOMAIN_NODE,nextpc);	
			}else if (more_power(current)){
				/******* MORE POWER ******/
				if (my_pc_opt.current_pc==my_pc_opt.def_powercap){
					/* That should the de typical use case. We want more power,current limit is not modified */
					uint TBR=compute_power_to_ask(current);
					my_pc_opt.requested=TBR;
					my_pc_opt.powercap_status=PC_STATUS_GREEDY;
				}else if (my_pc_opt.current_pc<my_pc_opt.last_t1_allocated){
					/* We have more power we can use it */
					set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);
				}else if (my_pc_opt.last_t1_allocated<my_pc_opt.def_powercap){
					/* We have less power allocated then the default */
	        uint pending;
 	      	my_pc_opt.powercap_status=PC_STATUS_ASK_DEF;
  	      pending=my_pc_opt.def_powercap-my_pc_opt.last_t1_allocated;
    	    my_pc_opt.requested=pending;
				}
			}
			break;
		case PC_STATUS_GREEDY:
			if (ok_power(current) || free_power(current)){
			/* We don't need more power, it was a phase */
				my_pc_opt.requested=0;
				my_pc_opt.powercap_status=PC_STATUS_OK;
			}
			break;
		case PC_STATUS_RELEASE:
			if (more_power(current)){
				/* We released some power but we want more now , go to last allocation, to simplify things*/	
				my_pc_opt.released=0;
				my_pc_opt.powercap_status=PC_STATUS_OK;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}else if (free_power(current)){
				/* we can still release more power */
				uint TBR,nextpc;
				TBR=compute_power_to_release(current);
				my_pc_opt.released+=TBR;
				nextpc=my_pc_opt.current_pc-TBR;
				set_powercap_value(DOMAIN_NODE,nextpc);
			}
			break;
		case PC_STATUS_ASK_DEF:
			if (ok_power(current) || free_power(current)){
				/* If we are ok, maybe we don't need more */
				my_pc_opt.powercap_status=PC_STATUS_OK;
				my_pc_opt.requested=0;
			}
			break;
	}
	pthread_mutex_unlock(&my_pc_opt.lock);
	return EAR_SUCCESS;
}
void get_powercap_status(powercap_status_t *my_status)
{
	debug("get_powercap_status");
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	my_status->idle_nodes=powermon_is_idle();
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_OK:my_status->num_greedy=0;break;
		case PC_STATUS_GREEDY:
			my_status->num_greedy=1; 
			/* How do deal with greedy vector */;break;
		case PC_STATUS_RELEASE:my_status->released_power=my_pc_opt.released;
		case PC_STATUS_ASK_DEF:
			my_status->num_newjob_nodes++;
			/*my_status->powerdef_nodes*/;break;
	}
	
	my_status->current_power=powermon_current_power();
	my_status->total_powercap=get_powercap_value();
	verbose(1,"powercap_status idle=%u current_power %u powercap %u",my_status->idle_nodes,my_status->current_power,my_status->total_powercap);
	pthread_mutex_unlock(&my_pc_opt.lock);
}


void print_powercap_opt(powercap_opt_t *opt)
{
	int i;
	debug("print_powercap_opt");
	fprintf(stderr,"num_greedy %u\n",opt->num_greedy);	
	for (i=0;i<opt->num_greedy;i++){
		fprintf(stderr,"greedy_node %d extra power %u\n",opt->greedy_nodes[i],opt->extra_power[i]);
	}
	fprintf(stderr,"Max extra power for new jobs %u\n",opt->max_inc_new_jobs);
}

void set_powercap_opt(powercap_opt_t *opt)
{
	print_powercap_opt(opt);
	
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	my_pc_opt.max_inc_new_jobs=opt->max_inc_new_jobs;
	/* Here we must check, based on our status, if actions must be taken */
  switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
    case PC_STATUS_OK:
    case PC_STATUS_RELEASE:
			debug("My powercap status is %u, doing nothing ",my_pc_opt.powercap_status);
			break;
    case PC_STATUS_GREEDY:
			debug("My status is GREEDY and new powercap opt received");
			int i;
			for (i=0;i<opt->num_greedy;i++){
				if (opt->greedy_nodes[i]==my_ip){
					my_pc_opt.last_t1_allocated=my_pc_opt.def_powercap+opt->extra_power[i];
					set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);
					/* If we are not using more than the default, we are ok, not greedy node */
					if (my_pc_opt.last_t1_allocated==my_pc_opt.def_powercap){
						my_pc_opt.powercap_status=PC_STATUS_OK;
					}
					break;
				}
			}
			break;
    case PC_STATUS_ASK_DEF:
			/* In that case, we assume we cat use the default power cap */
			debug("My status is ASK_DEF and new settings received");
			my_pc_opt.powercap_status=PC_STATUS_OK;
			my_pc_opt.last_t1_allocated=my_pc_opt.def_powercap;
			set_powercap_value(DOMAIN_NODE,my_pc_opt.def_powercap);			
			break;
  }
	pthread_mutex_unlock(&my_pc_opt.lock);

}

