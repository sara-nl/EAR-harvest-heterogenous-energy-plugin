/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*   
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*   
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING  
*/
#define _GNU_SOURCE 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <common/config.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/colors.h>
#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/eard_rapi.h>
#include <global_manager/cluster_powercap.h>


#if POWERCAP

extern cluster_conf_t my_cluster_conf;
extern uint  max_cluster_power;
extern uint  cluster_powercap_period;

uint num_nodes;
uint total_req_new,total_req_greedy,req_no_extra,num_no_extra,num_greedy,num_extra,extra_power_alloc,total_extra_power;
cluster_powercap_status_t *my_cluster_power_status;
int num_power_status;
powercap_opt_t cluster_options;
extern uint policy;

#define min(a,b) (a<b?a:b)

void aggregate_data(powercap_status_t *cs)
{	int i;
  total_req_greedy=0;
  total_extra_power=0;
	req_no_extra=0;
	num_no_extra=0;
	num_greedy=0;
	num_extra=0;extra_power_alloc=0;
	for (i=0;i<cs->num_greedy;i++){
		total_req_greedy+=cs->greedy_req[i];
		total_extra_power+=cs->extra_power[i];
		if (cs->greedy_req[i]) num_greedy++;
    if ((cs->greedy_req[i]) && (!cs->extra_power[i])){
      req_no_extra+=cs->greedy_req[i];
      num_no_extra++;
    }else if ((cs->greedy_req[i]) && (cs->extra_power[i])){ 
				num_extra++;
				extra_power_alloc+=cs->extra_power[i];
		}
	}
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


#if 0
void aggregate_powercap_status(powercap_status_t *my_cluster_power_status,int num_st,cluster_powercap_status_t *cluster_status)
{
  int i,num_greedy=0,num_def=0;
  uint total_req_greedy,total_extra_power;
  memset(cluster_status,0,sizeof(cluster_powercap_status_t));
  for (i=0;i<num_st;i++){
    if (my_cluster_power_status[i].num_greedy) cluster_status->num_greedy++;
    if (my_cluster_power_status[i].idle_nodes) cluster_status->idle_nodes++;
    cluster_status->released+=my_cluster_power_status[i].released;
    cluster_status->requested+=my_cluster_power_status[i].requested;
    cluster_status->current_power+=my_cluster_power_status[i].current_power;
    cluster_status->total_powercap+=my_cluster_power_status[i].total_powercap;
  }
  cluster_status->greedy_nodes=calloc(cluster_status->num_greedy,sizeof(int));
  cluster_status->greedy_req=calloc(cluster_status->num_greedy,sizeof(int));
  cluster_status->extra_power=calloc(cluster_status->num_greedy,sizeof(int));
  total_req_greedy=0;
  total_extra_power=0;
  for (i=0;i<num_st;i++){
    if (my_cluster_power_status[i].num_greedy){
      cluster_status->greedy_nodes[num_greedy]=my_cluster_power_status[i].greedy_nodes;
      cluster_status->greedy_req[num_greedy]=my_cluster_power_status[i].greedy_req;
      cluster_status->extra_power[num_greedy]=my_cluster_power_status[i].extra_power;
      num_greedy++;
      total_req_greedy+=cluster_status->greedy_req[num_greedy];
      total_extra_power+=cluster_status->extra_power[num_greedy];
    }
  }
}
#endif
void allocate_free_power_to_greedy_nodes(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options,uint *total_free)
{ 
  int i,more_power;
  uint pending=*total_free;
	if (num_greedy==0){
		debug("NO greedy nodes, returning");
		return;
	}
	if (pending==0){
		debug("NO free power, returning");
		return;
	}
	debug("allocate_free_power_to_greedy_nodes----------------");
	verbose(0,"Total extra power for nodes with NO current extra power %u W, %u nodes",req_no_extra,num_no_extra);
	verbose(0,"Nodes with extra power already allocated %u",num_greedy);
	if (num_no_extra>0){
  	if (req_no_extra<pending) more_power=-1;
  	else more_power=pending/num_no_extra;
  	verbose(0,"STAGE_1-Allocating %d watts to new greedy nodes first (-1 => alloc=req)",more_power);
  	for (i=0;i<cluster_status->num_greedy;i++){
    	if ((cluster_status->greedy_req[i]) && (!cluster_status->extra_power[i])){
      	if (more_power<0) cluster_options->extra_power[i]=cluster_status->greedy_req[i];
      	else cluster_options->extra_power[i]=min(more_power,cluster_status->greedy_req[i]);
      	pending-=cluster_options->extra_power[i];
    	}
  	}
	}
  /* If there is pending power to allocate */
	verbose(0,"STAGE-2 allocating %uW to the rest of greedy nodes",pending);
  if (pending){
    verbose(0,"Allocating %u watts to greedy nodes ",pending);
    more_power=pending/num_greedy;
    for (i=0;i<cluster_status->num_greedy;i++){
      if ((cluster_status->greedy_req[i]) && (!cluster_options->extra_power[i])){
        cluster_options->extra_power[i]=min(cluster_status->greedy_req[i],more_power);
        pending-=cluster_options->extra_power[i];
      }
    }
  }
  *total_free=pending;
}

/* This function is executed when there is not enough power for new running nodes */
void reduce_allocation(cluster_powercap_status_t *cs,powercap_opt_t *cluster_options,uint min_reduction)
{
	int i=0;
	uint red1,red,red_node;
	int new_extra;
	if (num_extra==0){
		error("We need to reallocated power and there is no extra power ");
		return;
	}
	red_node=min_reduction/num_extra;
	verbose(0,"SEQ reduce_allocation implementation avg red=%uW",red_node);
	debug("ROUND 1- reducing avg power ");
	while((min_reduction>0) && (i<cs->num_greedy)){
		if (cs->extra_power[i]){
			red1=min(red_node,cs->extra_power[i]);
			red=min(red1,min_reduction);
			new_extra=-red;
			verbose(0,"%sreducing %d W to node %d %s",COL_RED,new_extra,i,COL_CLR);
			cluster_options->extra_power[i]=new_extra;
			min_reduction-=red;
		}
		i++;
	}
	debug("ROUND 2- Reducing remaining power ");
	i=0;
	while((min_reduction>0) && (i<cs->num_greedy)){
		/* cluster_options->extra_power[i] is a negative value */
    if ((cs->extra_power[i]+cluster_options->extra_power[i])>0){
      red1=cs->extra_power[i]+cluster_options->extra_power[i];;
			red=min(red1,min_reduction);
			new_extra=-red;
      verbose(0,"%sreducing %d W to node %d %s",COL_RED,new_extra,i,COL_CLR);
      cluster_options->extra_power[i]=new_extra;
      min_reduction-=red;
    }
    i++;
  }

}

/* This function is executed when there is enough power for new running nodes but not for all the greedy nodes */
uint powercap_reallocation(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options,uint released)
{
  uint total_free;
	uint num_nodes=cluster_status->total_nodes;
  uint min_reduction;
  verbose(0,"There are %u nodes  %u idle nodes %u greedy nodes ",
  cluster_status->total_nodes,cluster_status->idle_nodes,cluster_status->num_greedy);
  memset(cluster_options,0,sizeof(powercap_opt_t));
  cluster_options->num_greedy=cluster_status->num_greedy;
  memcpy(cluster_options->greedy_nodes,cluster_status->greedy_nodes,cluster_status->num_greedy*sizeof(int));
	total_free=max_cluster_power-cluster_status->total_powercap;
  verbose(0,"Total power %u , requested for new %u released %u extra_req %u extra_used %u",max_cluster_power,cluster_status->requested,cluster_status->released,total_req_greedy,total_extra_power);
	verbose(0,"Free power before reallocation %u",total_free);
  /* Allocated power + default requested must be less that maximum */
  if ((cluster_status->total_powercap+cluster_status->requested)<=max_cluster_power){
    verbose(0,"There is enough power for new running jobs");
    total_free=max_cluster_power-(cluster_status->total_powercap+cluster_status->requested);
		verbose(0,"Free power after allocating power to new jobs %u",total_free);
		if (total_req_greedy==0){
			if ((total_free) && (cluster_status->idle_nodes>0))  cluster_options->max_inc_new_jobs=total_free/cluster_status->idle_nodes;
			return 1;
		}
		/* At this point we know there is greedy power requested */
    if (total_req_greedy<=total_free) {
      verbose(0,"There is more free power than requested by greedy nodes=> allocating all the req power");
      total_free-=total_req_greedy;
      memcpy(cluster_options->extra_power,cluster_status->greedy_req,cluster_status->num_greedy*sizeof(uint));
    }else{
      verbose(0,"There is not enough power for all the greedy nodes (free %u req %u)(used %u allocated %u)",total_free,total_req_greedy,cluster_status->current_power,cluster_status->total_powercap);
			if (released){
				verbose(0,"Anyway there is not enough power ");
      	cluster_options->num_greedy=cluster_status->num_greedy;
      	allocate_free_power_to_greedy_nodes(cluster_status,cluster_options,&total_free);
			}else return 0;
    }
  }else{
    /* There is not enough power for new jobs, we must reduce the extra allocation */
		if (released){
    	verbose(0,"We must reduce the extra allocation (used %u allocated %u)",cluster_status->current_power,cluster_status->total_powercap);
    	min_reduction=max_cluster_power-(cluster_status->total_powercap+cluster_status->requested);
    	total_free=0;
    	reduce_allocation(cluster_status,cluster_options,min_reduction);
		}else return 0;
  }
  if ((total_free) && (cluster_status->idle_nodes>0))  cluster_options->max_inc_new_jobs=total_free/cluster_status->idle_nodes;
	return 1;
}

void send_powercap_options_to_cluster(powercap_opt_t *cluster_options)
{
  cluster_set_powercap_opt(my_cluster_conf,cluster_options);
}

#if 0
void print_cluster_power_status(int num_power_status,powercap_status_t *my_cluster_power_status)
{
  int i;
  fprintf(stderr,"%d power_status received\n",num_power_status);
  for (i=0;i<num_power_status;i++){
    fprintf(stderr,"Node %d\n",i);
    fprintf(stderr,"\tidle nodes %u num_greedy %u \n",my_cluster_power_status[i].idle_nodes,my_cluster_power_status[i].num_greedy);
    fprintf(stderr,"\tgreedy_req %u new_req %u current_power %u total_powercap %u\n",
    my_cluster_power_status[i].greedy_req,my_cluster_power_status[i].requested,
    my_cluster_power_status[i].current_power,my_cluster_power_status[i].total_powercap);
  }
}
#endif

void print_cluster_power_status(powercap_status_t *my_cluster_power_status)
{
  int i;
	powercap_status_t *cs=my_cluster_power_status;
	if (cs==NULL){
		debug("cluster status is NULL");
		return;
	}
	debug("Total %u Idle  %u power (released %u requested %u consumed %u allocated %u) total_greedy_nodes %u",cs->total_nodes,cs->idle_nodes, cs->released,cs->requested,cs->current_power,cs->total_powercap,cs->num_greedy);
  debug("%d power_status received",num_power_status);
  for (i=0;i<cs->num_greedy;i++){
   debug("\t[%d] ip %d greedy_req %u extra_power %u",i,cs->greedy_nodes[i],cs->greedy_req[i],cs->extra_power[i]);
  }
}

pthread_t cluster_powercap_th;

void * eargm_powercap_th(void *noarg)
{
	if (pthread_setname_np(pthread_self(), "cluster_powercap")) error("Setting name forcluster_powercap thread %s", strerror(errno));

	#if POWERCAP
	while(1)
	{
		sleep(cluster_powercap_period);
		if (cluster_power_limited()){
			cluster_check_powercap();
		}

	}
	#endif

}

void cluster_powercap_init(cluster_conf_t *cc)
{
	int ret;
  if (max_cluster_power>0){
    verbose(0,"Power cap limit set to %u",max_cluster_power);
  }else{
    verbose(0,"Power cap unlimited");
  }

	if (max_cluster_power==0) return;

  /* This thread accepts external commands */
  if ((ret=pthread_create(&cluster_powercap_th, NULL, eargm_powercap_th, NULL))){
        errno=ret;
    	error("error creating eargm_server for external api %s",strerror(errno));
  }
}

int cluster_power_limited()
{
	return (max_cluster_power);
}


void cluster_check_powercap()
{
				debug("%sSTART cluster_check_powercap---------%s",COL_BLU,COL_CLR);
        num_power_status=cluster_get_powercap_status(&my_cluster_conf,&my_cluster_power_status);
				if (num_power_status==0){
					verbose(1,"num_power_status in cluster_check_powercap is 0");
					return;
				}
        print_cluster_power_status(my_cluster_power_status);
				aggregate_data(my_cluster_power_status);
        if (powercap_reallocation(my_cluster_power_status,&cluster_options,0)==0){
		      pc_release_data_t rel_power;
		      if (cluster_release_idle_power(&my_cluster_conf,&rel_power)==0){
	  	      error("Error in cluster_release_idle_power");
		      } else{
        		verbose(0,"%u Watts from idle nodes released",rel_power.released);
					}
        	my_cluster_power_status->released+=rel_power.released;
					powercap_reallocation(my_cluster_power_status,&cluster_options,1);
      	}

				print_powercap_opt(&cluster_options);
        send_powercap_options_to_cluster(&cluster_options);
				free(my_cluster_power_status);
				debug("%sEND cluster_check_powercap----------%s",COL_BLU,COL_CLR);
}
#endif
