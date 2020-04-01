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
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/system/execute.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/power_monitor.h>
#include <daemon/powercap.h>
#include <common/types/configuration/cluster_conf.h>

#define POWERCAP_MON 1

/* This version is a prototype and should be replaced by an INM plugin+GPU commands for powercap */

#define INM_ENABLE_CMD	"ipmitool raw 0x3a 0xc7 0x01"
#define INM_ENABLE_XCC_BRIGE "ipmitool raw 0x06 0x32 0 1"
#define INM_ENABLE_POLICY_CONTROL_CMD "ipmitool -b 0x00 -t 0x2c nm control enable global" 
#define INM_DISABLE_POLICY_CONTROL_CMD "ipmitool -b 0x00 -t 0x2c nm control disable global" 
#define INM_ENABLE_POWERCAP_POLICY_CMD "ipmitool -b 0x00 -t 0x2c nm policy add policy_id 0 domain platform correction soft power %d  trig_lim 1000 stats 30 enable"
#define INM_DISABLE_POWERCAP_POLICY_CMD "ipmitool -b 0x00 -t 0x2c nm policy remove policy_id 0"
#define INM_GET_POWERCAP_POLICY_CMD "NO_CMD"

#if 0
#define INM_DISABLE_POLICY_CONTROL_CMD "ipmitool -b 0 -t 0x2c raw 0x2E 0XC0 0X57 01 00 00 00 00" 
#define INM_ENABLE_POLICY_CONTROL_CMD "ipmitool -b 0 -t 0x2c raw 0x2E 0XC0 0X57 01 00 01 00 00" 
#define INM_SET_POWERCAP_VALUE_CMD "ipmitool -v -b 0 -t 0x2c raw 0x2E 0xD0 0x57 0x01 0x00 %#X %#X %#X"
#define INM_GET_POWERCAP_CMD "NO_CMD"
#endif

static node_powercap_opt_t my_pc_opt;
static int my_ip;
extern int *ips;
extern int self_id;
extern volatile int init_ips_ready;
int last_status;
int fd_powercap_values=0;

pthread_t powercapmon_th;
unsigned long powercapmon_freq=1;
extern int eard_must_exit;

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
uint compute_power_to_release(uint current)
{
	return my_pc_opt.th_release;
}

uint compute_power_to_ask(uint current)
{
	return my_pc_opt.th_release;
}

uint more_power(uint current)
{
	int dist;
  dist=(int)my_pc_opt.current_pc-(int)current;
	debug("More Power check:Computed distance %d",dist);
	if (dist<0) return 1;
	if (dist<my_pc_opt.th_inc) return 1;
	return 0;
}
uint free_power(uint current)
{
	int dist;
  dist=(int)my_pc_opt.current_pc-(int)current;
	debug("Free Power check:Computed distance %d",dist);
	if (dist<0) return 0;
	if (dist>=my_pc_opt.th_red) return 1;
	return 0;
}
uint ok_power(uint current)
{
	int dist;
	dist=(int)my_pc_opt.current_pc-(int)current;
	debug("OK Power check:Computed distance %d",dist);
	if (dist<0) return 0;
	if ((dist<my_pc_opt.th_red) && (dist>my_pc_opt.th_inc)) return 1;
	return 0;
}

int do_cmd(char *cmd)
{
	if (strcmp(cmd,"NO_CMD")==0) return 0;
	return 1;
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
	my_powercap_opt->powercap_idle=powermon_get_powercap_def()/2;
	my_powercap_opt->current_pc=powermon_get_powercap_def();
	my_powercap_opt->last_t1_allocated=powermon_get_powercap_def();
	my_powercap_opt->released=0;
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
	set_powercap_value(DOMAIN_NODE,0);
}

int powercap_init()
{
	char cmd[1024];

	debug("powercap init");
	set_default_node_powercap_opt(&my_pc_opt);
	print_node_powercap_opt(&my_pc_opt);
	while(init_ips_ready==0){ 
		sleep(1);
	}
	if (init_ips_ready>0) my_ip=ips[self_id];
	else my_ip=0;
	
	/* 1-Enable XCC-Bridge comm */
	debug("Enable XCC-Bridge");
	sprintf(cmd,INM_ENABLE_XCC_BRIGE);
	if (do_cmd(cmd)){
	if (execute(cmd)!=EAR_SUCCESS){ 
		debug("Error executing INM XCC-bridge");
		return EAR_ERROR;
	}
	}
	/* 2-Enable INM commands */
	debug("Enable INM");
	sprintf(cmd,INM_ENABLE_CMD);
	if (do_cmd(cmd)){
	if (execute(cmd)!=EAR_SUCCESS){ 
		debug("Error executing INM CMD enable");
		return EAR_ERROR;
	}
	}
	/* 3-Enable powercap policy control */
	sprintf(cmd,INM_ENABLE_POLICY_CONTROL_CMD);
	debug("Enable INM policy control");
	if (do_cmd(cmd)){
	if (execute(cmd)!=EAR_SUCCESS){
		debug("Error executing INM CMD Policy control");
		return EAR_ERROR;
	}
	}
	my_pc_opt.powercap_status=PC_STATUS_IDLE;
	last_status=PC_STATUS_IDLE;
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
	return EAR_SUCCESS;	
}

int is_powercap_set()
{
	/* 0 means unlimited */
	return (my_pc_opt.current_pc!=0);
}

int is_powercap_on()
{
	return (is_powercap_set() && (my_pc_opt.powercap_status!=PC_STATUS_ERROR));
}

uint get_powercap_value()
{
	return my_pc_opt.current_pc;
}

int set_powercap_value(uint domain,uint limit)
{
	char cmd[1024];
	char c_date[128];
	state_t ret;
	if (limit==my_pc_opt.current_pc) return EAR_SUCCESS;
	debug("set_powercap_value domain %u limit %u",domain,limit);
	get_date_str(c_date,sizeof(c_date));
	if (fd_powercap_values>=0){ 
		dprintf(fd_powercap_values,"%s domain %u limit %u \n",c_date,domain,limit);
	}
	sprintf(cmd,INM_ENABLE_POWERCAP_POLICY_CMD,limit);
	my_pc_opt.current_pc=limit;
	fprintf(stderr,"--------------- executing %s\n",cmd);
	return execute(cmd);
}


int powercap_idle_to_run()
{
	uint next_pc;
	uint extra;
	if (!is_powercap_on()) return EAR_SUCCESS;
	while(pthread_mutex_trylock(&my_pc_opt.lock)); /* can we create some deadlock because of status ? */
	last_status=PC_STATUS_IDLE;
	if (my_pc_opt.last_t1_allocated==my_pc_opt.powercap_idle){
		extra=my_pc_opt.max_inc_new_jobs;
	}else extra=0;
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
		debug("Goin from idle to run ");
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
	if (!is_powercap_on()) return EAR_SUCCESS;
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
		error("going from run to idle and we were in idle");
    break;
    case PC_STATUS_OK:
    case PC_STATUS_GREEDY:
    case PC_STATUS_ASK_DEF:
		case PC_STATUS_RELEASE:
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
	if (!is_powercap_on()) return EAR_SUCCESS;
	while(pthread_mutex_trylock(&my_pc_opt.lock));
	debug("PM event, current power %u powercap %u allocated %u status %u released %u requested %u",
		current,my_pc_opt.current_pc,my_pc_opt.last_t1_allocated,my_pc_opt.powercap_status,my_pc_opt.released,
		my_pc_opt.requested);
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
			debug("Idle node, no actions");
			break;
		case PC_STATUS_OK:
			/******* FREE POWER ******/
			if (free_power(current) && (last_status!=PC_STATUS_IDLE)){
				debug("status PC_STATUS_OK and free_power");
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
				debug("status PC_STATUS_OK and more_power");
				if (my_pc_opt.current_pc>=my_pc_opt.def_powercap){
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
			}else{
				debug("status PC_STATUS_OK and ok_power");
			}
			break;
		case PC_STATUS_GREEDY:
			if (ok_power(current) || free_power(current)){
				debug("Status greedy and power ok or free power");
			/* We don't need more power, it was a phase */
				my_pc_opt.requested=0;
				my_pc_opt.powercap_status=PC_STATUS_OK;
			}
			break;
		case PC_STATUS_RELEASE:
			if (more_power(current)){
				debug("status release and more power");
				/* We released some power but we want more now , go to last allocation, to simplify things*/	
				my_pc_opt.released=0;
				my_pc_opt.powercap_status=PC_STATUS_OK;
				set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);	
			}else if (free_power(current)){
				debug("status release and free power ");
				/* we can still release more power */
				uint TBR,nextpc;
				TBR=compute_power_to_release(current);
				my_pc_opt.released+=TBR;
				nextpc=my_pc_opt.current_pc-TBR;
				set_powercap_value(DOMAIN_NODE,nextpc);
			}
			break;
		case PC_STATUS_ASK_DEF:
			if ((ok_power(current) || free_power(current)) && (last_status!=PC_STATUS_IDLE)){
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
	fprintf(stderr,"Power_status:Ilde %u released %u requested %u total greedy %u  current power %u total power cap %u\n",
	my_status->idle_nodes,my_status->released,my_status->requested,my_status->num_greedy,my_status->current_power,
	my_status->total_powercap);
	for (i=0;i<my_status->num_greedy;i++){
		if (my_status->num_greedy) fprintf(stderr,"greedy=(ip=%u,req=%u,extra=%u) ",my_status->greedy_nodes[i],my_status->greedy_req[i],my_status->extra_power[i]);
	}
}
void get_powercap_status(powercap_status_t *my_status)
{
	debug("get_powercap_status");
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	//memset(my_status,0,sizeof(powercap_status_t));
	switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
            my_status->idle_nodes++;
            my_pc_opt.released=0;
            my_pc_opt.last_t1_allocated=my_pc_opt.current_pc;
            break;
		case PC_STATUS_GREEDY:
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
            my_status->greedy_nodes[my_status->num_greedy - 1] = my_ip; /* IP missing */
			my_status->greedy_req[my_status->num_greedy - 1]=my_pc_opt.requested;
			if (my_pc_opt.last_t1_allocated>my_pc_opt.def_powercap) my_status->extra_power[my_status->num_greedy - 1]=my_pc_opt.last_t1_allocated-my_pc_opt.def_powercap;
            else my_status->extra_power[my_status->num_greedy - 1] = 0;
			break;
		case PC_STATUS_RELEASE:
            my_status->released+=my_pc_opt.released;
		    my_pc_opt.powercap_status=PC_STATUS_OK;my_pc_opt.released=0;my_pc_opt.last_t1_allocated=my_pc_opt.current_pc;break;
		case PC_STATUS_ASK_DEF: 
            /* Data management */
			my_status->requested=my_pc_opt.requested;
			break;
		case PC_STATUS_ERROR: break;
		case PC_STATUS_OK:
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
	my_status->total_powercap+=get_powercap_value();
	pthread_mutex_unlock(&my_pc_opt.lock);
	print_power_status(my_status);
}


void print_powercap_opt(powercap_opt_t *opt)
{
	int i;
	debug("print_powercap_opt");
	fprintf(stderr,"num_greedy %u\n",opt->num_greedy);	
	for (i=0;i<opt->num_greedy;i++){
		fprintf(stderr,"greedy_node %d extra power %d\n",opt->greedy_nodes[i],opt->extra_power[i]);
	}
	fprintf(stderr,"Max extra power for new jobs %u\n",opt->max_inc_new_jobs);
}

void set_powercap_opt(powercap_opt_t *opt)
{
	int i;
	print_powercap_opt(opt);
	if (!is_powercap_on()) return;	
	while(pthread_mutex_trylock(&my_pc_opt.lock)); 
	my_pc_opt.max_inc_new_jobs=opt->max_inc_new_jobs;
	/* Here we must check, based on our status, if actions must be taken */
  switch(my_pc_opt.powercap_status){
		case PC_STATUS_IDLE:
    case PC_STATUS_OK:
			if (my_pc_opt.last_t1_allocated>my_pc_opt.def_powercap){
				debug("set_powercap_opt status ok, looking for potential power reduction");	
      	for (i=0;i<opt->num_greedy;i++){
        debug("Comparing greedy node %d ip=%d with ip=%di extra %d",i,opt->greedy_nodes[i],my_ip,opt->extra_power[i]);
        if (opt->greedy_nodes[i]==my_ip){
          my_pc_opt.last_t1_allocated=my_pc_opt.last_t1_allocated+opt->extra_power[i];
          set_powercap_value(DOMAIN_NODE,my_pc_opt.last_t1_allocated);
          break;
        }
			}
			}
			break;
    case PC_STATUS_RELEASE:
			debug("My powercap status is %u, doing nothing ",my_pc_opt.powercap_status);
			break;
    case PC_STATUS_GREEDY:
			debug("My status is GREEDY and new powercap opt received ip=%d",my_ip);
			for (i=0;i<opt->num_greedy;i++){
				debug("Comparing greedy node %d ip=%d with ip=%d extra %d",i,opt->greedy_nodes[i],my_ip,opt->extra_power[i]);
				if (opt->greedy_nodes[i]==my_ip){
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
			debug("My status is ASK_DEF and new settings received");
			my_pc_opt.powercap_status=PC_STATUS_OK;
			/* We assume we will receive the requested power */
			my_pc_opt.last_t1_allocated=my_pc_opt.def_powercap;
			my_pc_opt.requested=0;
			set_powercap_value(DOMAIN_NODE,my_pc_opt.def_powercap);			
			break;
  }
	pthread_mutex_unlock(&my_pc_opt.lock);

}

