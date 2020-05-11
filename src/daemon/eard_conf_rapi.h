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



/**
*    \file remote_conf.h
*    \brief This file defines data types and constants shared by the client and server in the remote EAR API
*
*/



#ifndef REMOTE_CONF_H
#define REMOTE_CONF_H

#include <common/types/application.h>
#include <common/types/configuration/policy_conf.h>
#include <common/types/risk.h>

#define TEMP_NUM_NODES 10

typedef struct end_job_req{
	job_id jid;
	job_id sid;
	//int status;
}end_job_req_t;

typedef struct new_conf{
	ulong max_freq;
	ulong min_freq;
	ulong th;
	uint p_states;
	uint p_id;
}new_conf_t;

typedef struct new_policy_cont{
	char name[POLICY_NAME_SIZE];
	ulong def_freq;
	double settings[MAX_POLICY_SETTINGS];
}new_policy_cont_t;

typedef struct power_limit{
	unsigned int type;
	unsigned long limit;
}power_limit_t;

typedef struct risk_dec{
	risk_t level;
	unsigned long target;
}risk_dec_t;
typedef struct powercap_opt{
	uint num_greedy;			/* Number of greedy nodes */
	int greedy_nodes[TEMP_NUM_NODES]; 		/* List of greedy nodes */
	int extra_power[TEMP_NUM_NODES];		/* Extra power received by each greedy node */
	uint max_inc_new_jobs; /* Max power allowed to be received by new jobs since last powercap reported */
}powercap_opt_t;

typedef union req_data{
		application_t 		new_job;
		end_job_req_t 		end_job;
		new_conf_t 				ear_conf;
		new_policy_cont_t	pol_conf;
	    power_limit_t     pc;
		risk_dec_t 			risk;
		powercap_opt_t  pc_opt;
}req_data_t;



typedef struct request{
    uint        req;
    uint        node_dist;
    int        time_code;
    req_data_t  my_req;
}request_t;

typedef struct status_node_info{
	ulong avg_freq; // In KH
	ulong temp; // In degres, No creo que haya falta enviar un unsigned long long
	ulong power; // In Watts 
	ulong max_freq;// in KH
} status_node_info_t;

typedef struct app_info{
	uint job_id;
	uint step_id;
}app_info_t;

typedef struct request_header {
    int type;
    uint size;
} request_header_t;

typedef struct eard_policy_info{
    ulong freq; /* default freq in KH, divide by 1000000 to show Ghz */
    uint th;     /* th x 100, divide by 100 */
}eard_policy_info_t;



typedef struct status{
    unsigned int     ip;
    char    ok;
    status_node_info_t  node;
    app_info_t  app;
	unsigned int num_policies;
    eard_policy_info_t    policy_conf[TOTAL_POLICIES];
} status_t;

typedef struct performance{
	float cpi;
	float gbs;
	float gflops_watt;
}performance_t;

typedef struct powercap_status{
	uint total_nodes;
	uint idle_nodes; 	        /* Total number of idle nodes */
	uint released;        		/* Accumulated released power in last T1 */
  uint requested;         	/*accumulated new_req */
	uint num_greedy;          /* Number of greedy nodes */
	int *greedy_nodes;        /* List of greedy nodes */
	uint *greedy_req; 				/* extra_power requested */
  uint *extra_power;				/* extra power already allocated */
	#if POWERCAP_EXT
	performance_t	*perf_info;	/* Performance metrics used by powercap scheduler */
	#endif
	uint current_power; 	    /* Accumulated power */
  uint total_powercap;      /* Accumulated current powercap limits */
}powercap_status_t;


#define EAR_RC_NEW_JOB     0
#define EAR_RC_END_JOB     1
#define EAR_RC_MAX_FREQ    100
#define EAR_RC_NEW_TH	   101
#define EAR_RC_INC_TH	   102
#define EAR_RC_RED_PSTATE   103
#define EAR_RC_SET_FREQ 	104
#define EAR_RC_DEF_FREQ		105
#define EAR_RC_REST_CONF	106
#define EAR_RC_SET_POLICY 108
#define EAR_RC_SET_DEF_PSTATE 109
#define EAR_RC_SET_MAX_PSTATE 110
#define EAR_RC_PING		    500
#define EAR_RC_STATUS		600

/* New functions for power limits */
#define EAR_RC_RED_POWER 700
#define EAR_RC_SET_POWER 701 
#define EAR_RC_INC_POWER 702
#define EAR_RC_GET_POWER 703
#define EAR_RC_GET_POWERCAP_STATUS 	704
#define EAR_RC_SET_POWERCAP_OPT			705
#define EAR_RC_SET_RISK 800



#define EAR_TYPE_STATUS         2001
#define EAR_TYPE_POWER_STATUS   2002


#define NO_COMMAND 100000

#define STATUS_BAD      0
#define STATUS_OK       1

#define ABSOLUTE 0
#define RELATIVE 1


#else
#endif
