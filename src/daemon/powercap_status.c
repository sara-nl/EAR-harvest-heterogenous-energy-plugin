/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

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
//#define SHOW_DEBUGS 0
#include <common/output/verbose.h>
#include <common/hardware/frequency.h>
#include <common/states.h>
#include <daemon/powercap.h>
#include <common/types/pc_app_info.h>

uint compute_power_to_release(node_powercap_opt_t *pc_opt,uint current)
{
	return pc_opt->th_release;
}

uint compute_power_to_release_with_earl(node_powercap_opt_t *pc_opt,uint current,pc_app_info_t *app,ulong avg_f)
{
	/* Is signature is still not computed, we will not release the power */
	if (app->req_f==0) return 0;
	if (avg_f<app->req_f) return 0;
	/* we release the power progresivelly, we can be more aggresive */
	if ((app->req_power>0)  && (app->req_power<current)) return (current-app->req_power)/2;
	else return pc_opt->th_release;
}

uint compute_extra_power(uint current,uint diff,uint target)
{
	uint i=0;
	uint total=0;
	if (diff==0) return 0;
	if (target==1) total=25;
	else total=10;
	for (i=1;i<diff;i++){
		total=total+10;
	}
	return total;
}

uint compute_power_to_ask(node_powercap_opt_t *pc_opt,uint current,pc_app_info_t *app,ulong avg_f)
{
	ulong adapted_f;
  int curr_pstate,target_pstate,diff_pstates;
  if (app->req_f==0) return 0;
  adapted_f=frequency_closest_high_freq(avg_f,1);
  curr_pstate=frequency_closest_pstate(adapted_f);
  target_pstate=frequency_closest_pstate(app->req_f);
  diff_pstates=curr_pstate-target_pstate;
	
	return compute_extra_power(current,diff_pstates,target_pstate);
}

uint compute_power_to_ask_with_earl(node_powercap_opt_t *pc_opt,uint current,pc_app_info_t *app,ulong avg_f)
{
	ulong adapted_f;
	int curr_pstate,target_pstate,diff_pstates;
	if (app->req_f==0) return 0;
	adapted_f=frequency_closest_high_freq(avg_f,1);
	curr_pstate=frequency_closest_pstate(adapted_f);
	target_pstate=frequency_closest_pstate(app->req_f);
	diff_pstates=curr_pstate-target_pstate;
	debug("Computing extra power avg_f %lu adapted_f %lu cpstate %d tpstate %d diff %d",avg_f,adapted_f,curr_pstate,target_pstate,diff_pstates);
	//return pc_opt->th_release*diff_pstates;
	/* We ask power based on earl estimations if available */
	return compute_extra_power(current,diff_pstates,target_pstate);
}

uint more_power(node_powercap_opt_t *pc_opt,uint current)
{
	int dist;
  dist=(int)pc_opt->current_pc-(int)current;
	debug("More Power check:Computed distance %d",dist);
	if (dist<0) return 1;
	if (dist<pc_opt->th_inc) return 1;
	return 0;
}
uint free_power(node_powercap_opt_t *pc_opt,uint current)
{
	int dist;
  dist=(int)pc_opt->current_pc-(int)current;
	debug("Free Power check:Computed distance %d",dist);
	if (dist<0) return 0;
	if (dist>=pc_opt->th_red) return 1;
	return 0;
}
uint ok_power(node_powercap_opt_t *pc_opt,uint current)
{
	int dist;
	dist=(int)pc_opt->current_pc-(int)current;
	debug("OK Power check:Computed distance %d",dist);
	if (dist<0) return 0;
	if ((dist<pc_opt->th_red) && (dist>pc_opt->th_inc)) return 1;
	return 0;
}

int is_powercap_set(node_powercap_opt_t *pc_opt)
{ 
  /* 0 means unlimited */
  /* we are not checking hw configuration in this function */
  return (pc_opt->current_pc!=0);
}

int is_powercap_on(node_powercap_opt_t *pc_opt)
{
  return ((pc_opt->powercap_status!=PC_STATUS_ERROR) && is_powercap_set(pc_opt));
}

uint get_powercapopt_value(node_powercap_opt_t *pc_opt)
{
  /* we are not checking hw configuration in this function */
  return pc_opt->current_pc;
}

uint get_powercap_allocated(node_powercap_opt_t *pc_opt)
{
  /* we are not checking hw configuration in this function */
  return pc_opt->last_t1_allocated;
}



uint compute_power_status(node_powercap_opt_t *pc,uint current_power)
{
	if (pc->last_t1_allocated<pc->def_powercap) return PC_STATUS_ASK_DEF;
  if (ok_power(pc,current_power)) return PC_STATUS_OK;
  if (more_power(pc,current_power)) return PC_STATUS_GREEDY;
  if (free_power(pc,current_power)) return PC_STATUS_RELEASE;
	return PC_STATUS_OK;
}


uint compute_next_status(node_powercap_opt_t *pc,uint current_power,ulong eff_f, ulong req_f)
{
	uint pstate=compute_power_status(pc,current_power);
	switch (pstate){
		case PC_STATUS_RELEASE:if (eff_f==req_f) return PC_STATUS_RELEASE; break;
		case PC_STATUS_GREEDY:if (eff_f<req_f) return PC_STATUS_GREEDY; break;
		case PC_STATUS_OK:if (eff_f< req_f) return PC_STATUS_GREEDY;break;
		default: return pstate;
	}
	return PC_STATUS_OK;
}

void powercap_status_to_str(uint s,char *b)
{	
	switch (s){
	case PC_STATUS_OK:sprintf(b,"OK");break;
	case PC_STATUS_GREEDY:sprintf(b,"greedy");break;
	case PC_STATUS_RELEASE:sprintf(b,"release");break;
	case PC_STATUS_ASK_DEF:sprintf(b,"ask_def");break;
	case PC_STATUS_IDLE:sprintf(b,"idle");break;
	case PC_STATUS_STOP:sprintf(b,"stop");break;
	case PC_STATUS_START:sprintf(b,"start");break;
	case PC_STATUS_RUN:sprintf(b,"run");break;
	default:	sprintf(b,"undef");break;
	}
}
