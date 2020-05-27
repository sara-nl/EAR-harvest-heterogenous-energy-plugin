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
#include <errno.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <common/config.h>
#include <common/colors.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/system/execute.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/power_monitor.h>
#include <daemon/powercap.h>
#include <daemon/powercap_status.h>
#include <daemon/shared_configuration.h>
#include <daemon/power_mgt.h>
#include <common/types/configuration/cluster_conf.h>

#define POWERCAP_MON 0



static node_powercap_opt_t my_pc_opt;
static int my_ip;
extern int *ips;
extern int self_id;
extern volatile int init_ips_ready;
int last_status;
int fd_powercap_values=0;
static pwr_mgt_t *pcmgr;
static uint pc_pid=0;
static uint pc_status_config=AUTO_CONFIG;

pthread_t powercapmon_th;
unsigned long powercapmon_freq=1;
extern int eard_must_exit;
extern settings_conf_t *dyn_conf;
extern resched_t *resched_conf;
static uint pc_strategy;
extern pc_app_info_t *pc_app_info_data;

#define min(a,b) ((a<b)?a:b)



void get_date_str(char *msg,int size)
{
	struct tm *current_t;
  time_t rawtime;
	time(&rawtime);
	current_t = localtime(&rawtime);
  strftime(msg, size, "%c", current_t);
}

#if POWERCAP_MON
void * powercapmon_f(void * arg)
{
	unsigned long f=5;
	int fd;
	ehandler_t pc_eh;
  energy_data_t e_begin;
  energy_data_t e_end;
  power_data_t pc_power;

	//pthread_setname_np(pthread_self(),"powercap_monitor");
	
	fd=open("/var/run/ear/powercap_mon.txt",O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
	if (init_power_ponitoring(&pc_eh)!=EAR_SUCCESS){
		error("PC init_power_ponitoring");
		pthread_exit(0);
	}
	alloc_energy_data(&e_begin);
  alloc_energy_data(&e_end);
  alloc_power_data(&pc_power);
	read_enegy_data(&pc_eh, &e_begin);
	while (!eard_must_exit) {
		sleep(f);
		read_enegy_data(&pc_eh, &e_end);
		compute_power(&e_begin, &e_end, &pc_power);
		print_power(&pc_power,1,fd);
		copy_energy_data(&e_begin, &e_end);
	}
	pthread_exit(0);
}
#endif

void update_node_powercap_opt_shared_info()
{
	memcpy(&dyn_conf->pc_opt,&my_pc_opt,sizeof(node_powercap_opt_t));
	resched_conf->force_rescheduling=1;
}


void print_node_powercap_opt(node_powercap_opt_t *my_powercap_opt)
{
	fprintf(stderr,"cuurent %u pc_def %u pc_idle %u th_inc %u th_red %u th_release %u max_new_jobs %u requested %u released %u\n",
	my_powercap_opt->current_pc,my_powercap_opt->def_powercap,my_powercap_opt->powercap_idle,my_powercap_opt->th_inc,my_powercap_opt->th_red, my_powercap_opt->th_release,my_powercap_opt->max_inc_new_jobs,
	my_powercap_opt->requested,my_powercap_opt->released);
}

void set_default_node_powercap_opt(node_powercap_opt_t *my_powercap_opt)
{
	my_powercap_opt->def_powercap=powermon_get_powercap_def();
	my_powercap_opt->powercap_idle=powermon_get_powercap_def()*POWERCAP_IDLE_PERC;
	my_powercap_opt->current_pc=0;
	my_powercap_opt->last_t1_allocated=powermon_get_powercap_def();
	my_powercap_opt->released=my_powercap_opt->last_t1_allocated-my_powercap_opt->powercap_idle;
	my_powercap_opt->th_inc=10;
	my_powercap_opt->th_red=50;
	my_powercap_opt->th_release=25;
	my_powercap_opt->powercap_status=PC_STATUS_ERROR;
	my_powercap_opt->max_inc_new_jobs=0;
	my_powercap_opt->requested=0;
	pthread_mutex_init(&my_powercap_opt->lock,NULL);
}

void powercap_end()
{ 
	char cmd[1024];

	if (pmgt_disable(pcmgr)!=EAR_SUCCESS){
		error("pmgt_disable");
	}

}

int powercap_init()
{
	char cmd[1024];

	debug("powercap init");
	set_default_node_powercap_opt(&my_pc_opt);
	print_node_powercap_opt(&my_pc_opt);
	/* powercap set to 0 means unlimited */
	if (powermon_get_powercap_def()==0){ 
		debug("POWERCAP limit disabled");
		update_node_powercap_opt_shared_info();
		return EAR_SUCCESS;
	}
	while(init_ips_ready==0){ 
		sleep(1);
	}
	if (init_ips_ready>0) my_ip=ips[self_id];
	else my_ip=0;

	/* Low level power cap managemen initialization */
	if (pmgt_init()!=EAR_SUCCESS){
		error("Low level power capping management error");
		return EAR_ERROR;
	}
	if (pmgt_handler_alloc(&pcmgr)!=EAR_SUCCESS){
		error("Allocating memory for powercap handler");
		return EAR_ERROR;
	}
	if (pmgt_enable(pcmgr)!=EAR_SUCCESS){
		error("Initializing powercap manager");
		return EAR_ERROR;
	}

	/* End Low level power cap managemen initialization */
	my_pc_opt.powercap_status=PC_STATUS_IDLE;
	last_status=PC_STATUS_IDLE;
	pc_strategy=pmgt_get_powercap_strategy(pcmgr);
	#if 0
	if (pc_strategy==PC_DVFS) pmgt_set_pc_mode(pcmgr,PC_MODE_LIMIT);
	else pmgt_set_pc_mode(pcmgr,PC_MODE_TARGET);
	#endif
	pmgt_set_pc_mode(pcmgr,PC_MODE_TARGET);
	set_powercap_value(DOMAIN_NODE,my_pc_opt.powercap_idle);
	debug("powercap initialization finished");
	#if POWERCAP_MON
	fd_powercap_values=open("/var/run/ear/powercap_values.txt",O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
	if (fd_powercap_values<0){
		fprintf(stderr,"Error creating file for powercap_values %s\n",strerror(errno));
	}else{
		dprintf(fd_powercap_values,"File created\n");
		debug("/var/run/ear/powercap_values.txt created");
	}
		
	debug("Creating thread for powercap  monitor");
	if (pthread_create(&powercapmon_th,NULL,powercapmon_f,NULL)){
		error("Error creating powercapmon_th");
	}
	#endif
	update_node_powercap_opt_shared_info();
	return EAR_SUCCESS;	
}


int set_powercap_value(uint domain,uint limit)
{
	char cmd[1024];
	char c_date[128];
	state_t ret;
	if (limit==my_pc_opt.current_pc) return EAR_SUCCESS;
	debug("%sset_powercap_value domain %u limit %u%s",COL_BLU,domain,limit,COL_CLR);
	get_date_str(c_date,sizeof(c_date));
	if (fd_powercap_values>=0){ 
		dprintf(fd_powercap_values,"%s domain %u limit %u \n",c_date,domain,limit);
	}
	my_pc_opt.current_pc=limit;
	update_node_powercap_opt_shared_info();
	pmgt_set_app_req_freq(pcmgr,pc_app_info_data->req_f);
	return pmgt_set_powercap_value(pcmgr,pc_pid,domain,limit);
}


int powercap_idle_to_run()
{
	uint next_pc;
	uint extra;
	if (!is_powercap_on(&my_pc_opt)) return EAR_SUCCESS;
	debug("powercap_idle_to_run");
	while(pthread_mutex_trylock(&my_pc_opt.lock)); /* can we create some deadlock because of status ? */
	pmgt_set_status(pcmgr,PC_STATUS_RUN);
	//debug("pc status modified");
	last_status=PC_STATUS_IDLE;
	if (my_pc_opt.last_t1_allocated==my_pc_opt.powercap_idle){
		extra=my_pc_opt.max_inc_new_jobs;
	}else extra=0;
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
		debug("%sGoin from idle to run:allocated %u automatic_extra=%u %s ",COL_GRE,my_pc_opt.last_t1_allocated,my_pc_opt.max_inc_new_jobs,COL_CLR);
		/* There is enough power for me */
		if ((my_pc_opt.last_t1_allocated+extra)>=my_pc_opt.def_powercap){	
			/* if we already had de power, we just set the status as OK */
			if (my_pc_opt.last_t1_allocated>=my_pc_opt.def_powercap){
				my_pc_opt.powercap_status=PC_STATUS_OK;
				my_pc_opt.released=0;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}else{
				/* We must use extra power */
				my_pc_opt.last_t1_allocated=min(my_pc_opt.def_powercap,my_pc_opt.last_t1_allocated+extra);
				my_pc_opt.powercap_status=PC_STATUS_OK;
				my_pc_opt.released=0;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}	
		}else{ /* we must ask more power */
				uint pending;
				my_pc_opt.last_t1_allocated+=extra;
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
	if (!is_powercap_on(&my_pc_opt)) return EAR_SUCCESS;
	debug("powercap_run_to_idle");
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
		error("going from run to idle and we were in idle");
    break;
    case PC_STATUS_OK:
    case PC_STATUS_GREEDY:
    case PC_STATUS_ASK_DEF:
		case PC_STATUS_RELEASE:
		debug("%sGoin from run to idle%s",COL_GRE,COL_CLR);
		uint rel;
		my_pc_opt.released=my_pc_opt.last_t1_allocated-my_pc_opt.powercap_idle;
		my_pc_opt.requested=0;
		my_pc_opt.powercap_status=PC_STATUS_IDLE;
		set_powercap_value(DOMAIN_NODE,my_pc_opt.powercap_idle);
    break;
  }
	pmgt_set_status(pcmgr,PC_STATUS_IDLE);
  pthread_mutex_unlock(&my_pc_opt.lock);
  return EAR_SUCCESS;
}
int periodic_metric_info(dom_power_t *cp,uint use_earl,ulong avg_f)
{
	uint current=(uint)cp->platform;
	if (!is_powercap_on(&my_pc_opt)) return EAR_SUCCESS;
	debug("periodic_metric_info");
	while(pthread_mutex_trylock(&my_pc_opt.lock));
	pmgt_set_power_per_domain(pcmgr,cp);
	debug("%spc_app_info req_f %lu pc_status %u%s",COL_GRE,pc_app_info_data->req_f,pc_app_info_data->pc_status,COL_CLR);
	debug("%sPM event, current power %u powercap %u allocated %u status %u released %u requested %u%s",\
		COL_GRE,current,my_pc_opt.current_pc,my_pc_opt.last_t1_allocated,my_pc_opt.powercap_status,my_pc_opt.released,\
		my_pc_opt.requested,COL_CLR);
	/* If mode is AUTO_CONFIG EARD takes care of status changes, if not, the EARL will take care of it */
	if (pc_status_config!=AUTO_CONFIG){
		error("ONLY AUTO_CONFIG is supported for now, change and recompile");
	}
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
			debug("Idle node, no actions");
			break;
		case PC_STATUS_OK:
			/******* FREE POWER ******/
			if (free_power(&my_pc_opt,current) && (last_status!=PC_STATUS_IDLE)){
				debug("status PC_STATUS_OK and free_power");
				/* We are consuming few power, we can release son percentage */
				/* We must check the application status before relasing (and loosing) power */
				if ((use_earl && pc_app_info_data->pc_status==PC_STATUS_RELEASE) || (!use_earl)){
					uint TBR,nextpc;
					if (!use_earl) TBR=compute_power_to_release(&my_pc_opt,current);
					else TBR=compute_power_to_release_with_earl(&my_pc_opt,current,pc_app_info_data,avg_f);
					nextpc=my_pc_opt.current_pc-TBR; 
					my_pc_opt.released=TBR;
					my_pc_opt.requested=0;
					my_pc_opt.powercap_status=PC_STATUS_RELEASE;
					set_powercap_value(DOMAIN_NODE,nextpc);	
				}
			}else if (more_power(&my_pc_opt,current)){
				/******* MORE POWER ******/
				debug("status PC_STATUS_OK and more_power");
				if (my_pc_opt.current_pc>=my_pc_opt.def_powercap){
					/* That should the de typical use case. We want more power,current limit is not modified */
					uint TBR;
					if (!use_earl) TBR=compute_power_to_ask(&my_pc_opt,current);
					else TBR=compute_power_to_ask_with_earl(&my_pc_opt,current,pc_app_info_data,avg_f);
					if (TBR && ((use_earl && pc_app_info_data->pc_status==PC_STATUS_GREEDY) || (!use_earl))){
						my_pc_opt.requested=TBR;
						my_pc_opt.powercap_status=PC_STATUS_GREEDY;
					}
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
			}else{
				debug("status PC_STATUS_OK and ok_power");
				if (use_earl && pc_app_info_data->pc_status==PC_STATUS_GREEDY){
					debug("Going to GREEDY because of EARL");
          uint TBR;
          if (!use_earl) TBR=compute_power_to_ask(&my_pc_opt,current);
          else TBR=compute_power_to_ask_with_earl(&my_pc_opt,current,pc_app_info_data,avg_f);
					if (TBR){
						my_pc_opt.requested=TBR;
						my_pc_opt.powercap_status=PC_STATUS_GREEDY;
					}
				}
			}
			break;
		case PC_STATUS_GREEDY:
			if (ok_power(&my_pc_opt,current) || free_power(&my_pc_opt,current)){
				debug("Status greedy and power ok or free power");
				if ((use_earl && pc_app_info_data->pc_status==PC_STATUS_OK) || (!use_earl)){
					/* We don't need more power, it was a phase */
					my_pc_opt.requested=0;
					my_pc_opt.powercap_status=PC_STATUS_OK;
				}
			}
			break;
		case PC_STATUS_RELEASE:
			if (more_power(&my_pc_opt,current)){
				debug("status release and more power");
				/* We released some power but we want more now , go to last allocation, to simplify things*/	
				my_pc_opt.released=0;
				my_pc_opt.powercap_status=PC_STATUS_OK;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}else if (free_power(&my_pc_opt,current)){
				debug("status release and free power ");
				/* we can still release more power */
				uint TBR,nextpc;
        if (!use_earl) TBR=compute_power_to_release(&my_pc_opt,current);
        else TBR=compute_power_to_release_with_earl(&my_pc_opt,current,pc_app_info_data,avg_f);
				my_pc_opt.released+=TBR;
				nextpc=my_pc_opt.current_pc-TBR;
				set_powercap_value(DOMAIN_NODE,nextpc);
			}
			break;
		case PC_STATUS_ASK_DEF:
			if ((ok_power(&my_pc_opt,current) || free_power(&my_pc_opt,current)) && (last_status!=PC_STATUS_IDLE)){
				debug("status ask_def and ok power or free power");
				/* If we are ok, maybe we don't need more */
				my_pc_opt.powercap_status=PC_STATUS_OK;
				my_pc_opt.requested=0;
			}
			break;
	}
	debug("PM new state, current power %u powercap %u allocated %u status %u released %u requested %u",
		current,my_pc_opt.current_pc,my_pc_opt.last_t1_allocated,my_pc_opt.powercap_status,my_pc_opt.released,
		my_pc_opt.requested);
	last_status=my_pc_opt.powercap_status;
	pthread_mutex_unlock(&my_pc_opt.lock);
	return EAR_SUCCESS;
}
void print_power_status(powercap_status_t *my_status)
{
	int i;
	debug("Power_status:Ilde %u released %u requested %u total greedy %u  current power %u total power cap %u",
	my_status->idle_nodes,my_status->released,my_status->requested,my_status->num_greedy,my_status->current_power,
	my_status->total_powercap);
	for (i=0;i<my_status->num_greedy;i++){
		if (my_status->num_greedy) debug("greedy=(ip=%u,req=%u,extra=%u) ",my_status->greedy_nodes[i],my_status->greedy_req[i],my_status->extra_power[i]);
	}
}

void get_powercap_status(powercap_status_t *my_status)
{
	debug("get_powercap_status");
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	my_status->total_nodes++;
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
						debug("%sIdle%s node!, release %u allocated power %u",COL_BLU,COL_CLR,my_pc_opt.released,my_pc_opt.current_pc);
            my_status->idle_nodes++;
						#if 0
						my_status->released+=my_pc_opt.released;
            my_pc_opt.released=0;
            my_pc_opt.last_t1_allocated=my_pc_opt.current_pc;
						#endif
            break;
		case PC_STATUS_GREEDY:
					debug("%sGreedy%s node asking for %u watts current pc %u",COL_BLU,COL_CLR,my_pc_opt.requested,my_pc_opt.last_t1_allocated);
            /* Memory management */
            if (my_status->num_greedy < 1) {
                my_status->greedy_nodes=NULL;
                my_status->greedy_req=NULL;
                my_status->extra_power=NULL;
            }
			my_status->num_greedy++; 
            if (my_status->num_greedy < 1) break;
			my_status->greedy_nodes=realloc(my_status->greedy_nodes, sizeof(int)*my_status->num_greedy);
		    my_status->greedy_req=realloc(my_status->greedy_req, sizeof(uint)*my_status->num_greedy);
		    my_status->extra_power=realloc(my_status->extra_power, sizeof(uint)*my_status->num_greedy);
            /* Data management */
            my_status->greedy_nodes[my_status->num_greedy - 1] = my_ip; 
			my_status->greedy_req[my_status->num_greedy - 1]=my_pc_opt.requested;
			if (my_pc_opt.last_t1_allocated>my_pc_opt.def_powercap) my_status->extra_power[my_status->num_greedy - 1]=my_pc_opt.last_t1_allocated-my_pc_opt.def_powercap;
            else my_status->extra_power[my_status->num_greedy - 1] = 0;
			break;
		case PC_STATUS_RELEASE:
			debug("%sReleasing%s %u W allocated %u W",COL_BLU,COL_CLR,my_pc_opt.released,my_pc_opt.current_pc);
            my_status->released+=my_pc_opt.released;
		    my_pc_opt.powercap_status=PC_STATUS_OK;my_pc_opt.released=0;my_pc_opt.last_t1_allocated=my_pc_opt.current_pc;break;
		case PC_STATUS_ASK_DEF: 
            /* Data management */
			debug("%sAsking for defaul power%s %uW allocated %uW",COL_BLU,COL_CLR,my_pc_opt.requested,my_pc_opt.last_t1_allocated);
			my_status->requested+=my_pc_opt.requested;
			break;
		case PC_STATUS_ERROR: break;
		case PC_STATUS_OK:
			debug("%sNode OK%s allocated %u W extra=%uW",COL_BLU,COL_CLR,my_pc_opt.last_t1_allocated,my_pc_opt.last_t1_allocated-my_pc_opt.def_powercap);
			if (my_pc_opt.last_t1_allocated>my_pc_opt.def_powercap){
                if (my_status->num_greedy < 1) {
                    my_status->greedy_nodes=NULL;
                    my_status->greedy_req=NULL;
                    my_status->extra_power=NULL;
                }
			    my_status->num_greedy++; 
                if (my_status->num_greedy < 1) break;
			    my_status->greedy_nodes=realloc(my_status->greedy_nodes, sizeof(int)*my_status->num_greedy);
    		    my_status->greedy_req=realloc(my_status->greedy_req, sizeof(uint)*my_status->num_greedy);
	    	    my_status->extra_power=realloc(my_status->extra_power, sizeof(uint)*my_status->num_greedy);
                /* Data management */
                my_status->greedy_nodes[my_status->num_greedy - 1] = my_ip; /* IP missing */
    			my_status->greedy_req[my_status->num_greedy - 1]=0;
                my_status->extra_power[my_status->num_greedy - 1]=my_pc_opt.last_t1_allocated-my_pc_opt.def_powercap;
			}	
			break;
	}
	
	my_status->current_power+=powermon_current_power();
	my_status->total_powercap+=get_powercap_allocated(&my_pc_opt);
	pthread_mutex_unlock(&my_pc_opt.lock);
	print_power_status(my_status);
}

void copy_node_powercap_opt(node_powercap_opt_t *dst)
{
	memcpy(dst,&my_pc_opt,sizeof(node_powercap_opt_t));
}

void print_powercap_opt(powercap_opt_t *opt)
{
	int i;
	debug("num_greedy %u",opt->num_greedy);	
	for (i=0;i<opt->num_greedy;i++){
		debug("greedy_node %d extra power %d",opt->greedy_nodes[i],opt->extra_power[i]);
	}
	debug("Max extra power for new jobs %u",opt->max_inc_new_jobs);
}

void set_powercap_opt(powercap_opt_t *opt)
{
	int i;
	print_powercap_opt(opt);
	if (!is_powercap_on(&my_pc_opt)) return;	
	debug("set_powercap_opt command received");
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	my_pc_opt.max_inc_new_jobs=opt->max_inc_new_jobs;
	/* Here we must check, based on our status, if actions must be taken */
  switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
    case PC_STATUS_OK:
			if (my_pc_opt.last_t1_allocated>my_pc_opt.def_powercap){
				// debug("set_powercap_opt status ok, looking for potential power reduction");	
      	for (i=0;i<opt->num_greedy;i++){
        if (opt->greedy_nodes[i]==my_ip){
        	debug("%snew_pc_opt:status OK but in the greedy node list %d ip=%d with ip=%di extra %d%s",COL_RED,i,opt->greedy_nodes[i],my_ip,opt->extra_power[i],COL_CLR);
          my_pc_opt.last_t1_allocated=my_pc_opt.last_t1_allocated+opt->extra_power[i];
          set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);
          break;
        }
			}
			}
			break;
    case PC_STATUS_RELEASE:
			//debug("My powercap status is %u, doing nothing ",my_pc_opt.powercap_status);
			break;
    case PC_STATUS_GREEDY:
			// debug("My status is GREEDY and new powercap opt received ip=%d",my_ip);
			for (i=0;i<opt->num_greedy;i++){
				if (opt->greedy_nodes[i]==my_ip){
					debug("%s new_pc_opt: greedy node %d ip=%d with ip=%d extra %d%s",COL_GRE,i,opt->greedy_nodes[i],my_ip,opt->extra_power[i],COL_CLR);
					my_pc_opt.last_t1_allocated=my_pc_opt.last_t1_allocated+opt->extra_power[i];
					set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);
					my_pc_opt.powercap_status=PC_STATUS_OK;
					my_pc_opt.requested=0;
					break;
				}
			}
			break;
    case PC_STATUS_ASK_DEF:
			/* In that case, we assume we cat use the default power cap */
			// debug("My status is ASK_DEF and new settings received");
			my_pc_opt.powercap_status=PC_STATUS_OK;
			/* We assume we will receive the requested power */
			my_pc_opt.last_t1_allocated=my_pc_opt.def_powercap;
			my_pc_opt.requested=0;
			set_powercap_value(DOMAIN_NODE,my_pc_opt.def_powercap);			
			break;
  }
	pthread_mutex_unlock(&my_pc_opt.lock);

}


void set_powercapstatus_mode(uint mode)
{
	pc_status_config=mode;
}

uint powercap_get_strategy()
{
	return pmgt_get_powercap_strategy(pcmgr);
}

void powercap_set_app_req_freq(ulong f)
{
	pmgt_set_app_req_freq(pcmgr,f);
}

void powercap_release_idle_power(pc_release_data_t *release)
{
  while(pthread_mutex_trylock(&my_pc_opt.lock));
  switch(my_pc_opt.powercap_status){
  case PC_STATUS_IDLE:
  	debug("%sReleasing %u allocated IDLE power %s",COL_BLU,my_pc_opt.released,COL_CLR);
  	release->released+=my_pc_opt.released;
  	my_pc_opt.released=0;
  	my_pc_opt.last_t1_allocated=my_pc_opt.current_pc;
  	break;
	default:break;
	}
	pthread_mutex_unlock(&my_pc_opt.lock);
}
