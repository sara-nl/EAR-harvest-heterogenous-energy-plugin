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


//#define SHOW_DEBUGS 0
#include <common/output/verbose.h>
#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/eard_rapi.h>
#include <global_manager/cluster_powercap.h>


#if POWERCAP

extern cluster_conf_t my_cluster_conf;
uint max_cluster_power,default_power,num_nodes;
uint total_req_new,total_req_greedy;
cluster_powercap_status_t *my_cluster_power_status;
int num_power_status;
powercap_opt_t cluster_options;

#define min(a,b) (a<b?a:b)

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
  int i, nodes_no_extra=0, num_extra=0,num_greedy=0,more_power;
  uint pending=*total_free;
  for (i=0;i<cluster_status->num_greedy;i++){
    if ((cluster_status->greedy_req[i]) && (!cluster_status->extra_power[i])){
      nodes_no_extra+=cluster_status->greedy_req[i];
      num_extra++;
    }else num_greedy++;
  }
  if (nodes_no_extra<pending) more_power=-1;
  else more_power=pending/num_extra;
  verbose(0,"Allocating %d watts to new greedy nodes first (-1 => alloc=req",more_power);
  for (i=0;i<cluster_status->num_greedy;i++){
    if ((cluster_status->greedy_req[i]) && (!cluster_status->extra_power[i])){
      if (more_power<0) cluster_options->extra_power[i]=cluster_status->greedy_req[i];
      else cluster_options->extra_power[i]=min(more_power,cluster_status->greedy_req[i]);
      pending-=cluster_options->extra_power[i];
    }
  }
  /* If there is pending power to allocate */
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
void reduce_allocation(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options,uint min_reduction)
{
	verbose(0,"reduce_allocation not implemented");

}

/* This function is executed when there is enough power for new running nodes but not for all the greedy nodes */
void powercap_reallocation(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options)
{
  uint total_free;
	uint num_nodes=cluster_status->total_nodes;
  uint min_reduction;
  verbose(0,"There are %u nodes  %u idle nodes %u greedy nodes ",
  cluster_status->total_nodes,cluster_status->idle_nodes,cluster_status->num_greedy);
  memset(cluster_options,0,sizeof(powercap_opt_t));
  verbose(0,"Total power %u , requested for new %u released %u extra req %u",max_cluster_power,cluster_status->requested,cluster_status->released,total_req_greedy);
  /* Allocated power + default requested must be less that maximum */
  if ((cluster_status->total_powercap+cluster_status->requested)<=max_cluster_power){
    verbose(0,"There is enough power for new running");
    total_free=max_cluster_power-(cluster_status->total_powercap+cluster_status->requested);
    if (total_req_greedy>=total_free){
      verbose(0,"There is more free power than requested by greedy nodes=> allocating all the extra power");
      total_free-=total_req_greedy;
      cluster_options->num_greedy=cluster_status->num_greedy;
      memcpy(cluster_options->greedy_nodes,cluster_status->greedy_nodes,cluster_status->num_greedy*sizeof(int));
      memcpy(cluster_options->extra_power,cluster_status->greedy_req,cluster_status->num_greedy*sizeof(uint));
    }else{
      verbose(0,"There is not enough power for all the greedy nodes ");
      cluster_options->num_greedy=cluster_status->num_greedy;
      allocate_free_power_to_greedy_nodes(cluster_status,cluster_options,&total_free);
    }
  }else{
    /* There is not enough power for new jobs, we must reduce the extra allocation */
    verbose(0,"We must reduce the extra allocation");
    cluster_options->num_greedy=0;
    min_reduction=max_cluster_power-(cluster_status->total_powercap+cluster_status->requested);
    total_free=0;
    reduce_allocation(cluster_status,cluster_options,min_reduction);
  }
  if (total_free) cluster_options->max_inc_new_jobs=total_free/(num_nodes-cluster_status->idle_nodes);
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
	fprintf(stderr,"Total %u Idle  %u power (released %u requested %u consumed %u allocated %u) total_greedy_nodes %u\n",cs->total_nodes,cs->idle_nodes, cs->released,cs->requested,cs->current_power,cs->total_powercap,cs->num_greedy);
  fprintf(stderr,"%d power_status received\n",num_power_status);
  for (i=0;i<cs->num_greedy;i++){
   fprintf(stderr,"\t[%d] ip %d greedy_req %u extra_power %u\n",i,cs->greedy_nodes[i],cs->greedy_req[i],cs->extra_power[i]);
  }
}


void cluster_powercap_init()
{
  char *max_cluster_power_st=getenv("EAR_MAX_CLUSTER_POWER");
  char *default_power_st=getenv("EAR_DEF_POWER");
  if (max_cluster_power_st!=NULL) max_cluster_power=atoi(max_cluster_power_st);
  else max_cluster_power=0;
  if (default_power_st!=NULL) default_power=atoi(default_power_st);
  if (max_cluster_power>0){
    verbose(0,"Power cap limit set to %u",max_cluster_power);
  }else{
    verbose(0,"Power cap unlimited");
  }

}

int cluster_power_limited()
{
	return (max_cluster_power);
}


void cluster_check_powercap()
{
        num_power_status=cluster_get_powercap_status(&my_cluster_conf,&my_cluster_power_status);
        print_cluster_power_status(my_cluster_power_status);
        powercap_reallocation(my_cluster_power_status,&cluster_options);
        send_powercap_options_to_cluster(&cluster_options);

}
#endif
