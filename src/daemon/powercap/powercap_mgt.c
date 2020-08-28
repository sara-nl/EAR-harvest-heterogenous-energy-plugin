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
#include <common/config/config_install.h>
#include <common/states.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/symplug.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/shared_configuration.h>
#include <daemon/powercap/powercap_mgt.h>
#include <common/system/monitor.h>
#if USE_GPUS
#include <metrics/gpu/gpu.h>
#include <management/gpu/gpu.h>
#endif
#include <common/hardware/topology.h>
extern settings_conf_t *dyn_conf;



typedef struct powercap_symbols {
  state_t (*enable)        (suscription_t *sus);
  state_t (*disable)       ();
  state_t (*set_powercap_value)(uint pid,uint domain,uint limit,ulong *util);
  state_t (*get_powercap_value)(uint pid,ulong *powercap);
  uint    (*is_powercap_policy_enabled)(uint pid);
  void    (*set_status)(uint status);
	void		(*set_pc_mode)(uint mode);
  uint    (*get_powercap_strategy)();
  void    (*powercap_to_str)(char *b);
	void 		(*print_powercap_value)(int fd);
	void 		(*set_app_req_freq)(ulong *f);
	void 		(*set_verb_channel)(int fd);	
	void 		(*set_new_utilization)(ulong *util);	
	uint    (*get_powercap_status)(uint *in_target,uint *tbr);
} powercapsym_t;

/*****
#define DOMAIN_NODE 0
#define DOMAIN_CPU 1
#define DOMAIN_DRAM     2
#define DOMAIN_GPU      3
****/

static dom_power_t pdist_per_domain;
static uint pmgt_limit;
static float pdomains[NUM_DOMAINS]={0.6,0.45,0,GPU_PERC_UTIL};
static float pdomains_idle[NUM_DOMAINS]={0.6,0.45,0,GPU_PERC_UTIL};
static ulong *current_util[NUM_DOMAINS],*prev_util[NUM_DOMAINS];
static ulong  dom_util[NUM_DOMAINS],last_dom_util[NUM_DOMAINS],dom_changed[NUM_DOMAINS];
#if USE_GPUS
static float pdomains_def_nogpus[NUM_DOMAINS]={0.6,0.45,0,GPU_PERC_UTIL};
static float pdomains_def_withgpus[NUM_DOMAINS]={0.5,0.4,0,GPU_PERC_UTIL};
#else
static float pdomains_def_nogpus[NUM_DOMAINS]={1.0,0.85,0,0};
#endif
static uint domains_loaded[NUM_DOMAINS]={0,0,0,0};
static powercapsym_t pcsyms_fun[NUM_DOMAINS];
static void *pcsyms_obj[NUM_DOMAINS] ={ NULL,NULL,NULL,NULL};
const int   pcsyms_n = 14;
extern cluster_conf_t my_cluster_conf;

static suscription_t *sus[NUM_DOMAINS];

const char     *pcsyms_names[] ={
  "enable",
  "disable",
  "set_powercap_value",
  "get_powercap_value",
  "is_powercap_policy_enabled",
  "set_status",
	"set_pc_mode",
  "get_powercap_strategy",
  "powercap_to_str",
	"print_powercap_value",
	"set_app_req_freq",	
	"set_verb_channel",
	"set_new_utilization",
	"get_powercap_status"
};




#define freturn(call, ...) ((call==NULL)?EAR_UNDEFINED:call(__VA_ARGS__));

#define DEFAULT_PC_PLUGIN_NAME_NODE "noplugin"
#define DEFAULT_PC_PLUGIN_NAME_CPU  "dvfs"
#define DEFAULT_PC_PLUGIN_NAME_DRAM "noplugin"
#if USE_GPUS
#define DEFAULT_PC_PLUGIN_NAME_GPU  "gpu_dvfs"
#else
#define DEFAULT_PC_PLUGIN_NAME_GPU  "noplugin"
#endif
static uint pc_plugin_loaded=0;

#if USE_GPUS
static suscription_t *sus_util_detection;
static gpu_t *gpu_detection_raw_data;
static ctx_t gpu_pc;
static uint gpu_pc_num_gpus=0;
static uint gpu_pc_model = 0;
#else
static uint gpu_pc_num_gpus=1;
#endif
static topology_t pc_topology_info;
/* These functions identies and monitors load changes */

#define MAX_ALLOWED_DIFF 0.1

static uint gpu_util_changed(ulong curr,ulong prev)
{
	float diff;
	int idiff,icurr,iprev;	
	icurr=(int)curr;
	iprev=(int)prev;
	if ((prev == 0) && (curr)) return 1;
	if ((curr == 0) && (prev)) return 1;
	if ((!prev) && (!curr)) return 0;
	idiff = (icurr - iprev);
	if (idiff < 0){ 
		idiff = idiff * -1;
		diff = (float)idiff / (float)iprev;
	}else{
		diff = (float)idiff / (float)icurr;
	}
	// debug("Current util and prev util differs in %.3f prev %lu curr %lu",diff,prev,curr);
	return (diff > 	MAX_ALLOWED_DIFF);
}

uint pmgt_utilization_changed()
{
	uint ret=0,i,g;
	for (i=0;i<NUM_DOMAINS;i++){
		dom_changed[i]=0;
		if (dom_util[i] != last_dom_util[i]){ 
			ret=1;
			dom_changed[i]=1;
		}
		#if USE_GPUS
		if ((i == DOMAIN_GPU ) && (gpu_pc_model != MODEL_DUMMY)){
				for (g=0;g<gpu_pc_num_gpus;g++){
					//debug("Comparing utilization for GPU %d : %lu vs %lu",g,current_util[DOMAIN_GPU][g],prev_util[DOMAIN_GPU][g]);
					if (gpu_util_changed(current_util[DOMAIN_GPU][g],prev_util[DOMAIN_GPU][g])){ 
						ret =1;
						dom_changed[i]=1;
					}
				}
		}
		#endif
	}
	return ret;
}
static state_t util_detect_main(void *p)
{
  int i;
	dom_util[DOMAIN_NODE] = 1;
	dom_util[DOMAIN_CPU] = pc_topology_info.cpu_count;
	dom_util[DOMAIN_DRAM] = pc_topology_info.socket_count;
	dom_util[DOMAIN_GPU]=0;
	#if USE_GPUS
  if (gpu_read_raw(&gpu_pc,gpu_detection_raw_data) != EAR_SUCCESS){
    error("Error reading GPU data in powercap");
  }
  for (i=0;i<gpu_pc_num_gpus;i++){
    // dom_util[DOMAIN_GPU] += gpu_detection_raw_data[i].util_gpu;
    /* Using the utilization is too fine grain */
    dom_util[DOMAIN_GPU] += (gpu_detection_raw_data[i].working > 0);
		prev_util[DOMAIN_GPU][i] = current_util[DOMAIN_GPU][i];
    current_util[DOMAIN_GPU][i]=gpu_detection_raw_data[i].util_gpu;
		
  }
	#endif
	if (pmgt_utilization_changed()){
    pmgt_powercap_node_reallocation();
  }
	memcpy(last_dom_util,dom_util,sizeof(ulong)*NUM_DOMAINS);
  return EAR_SUCCESS;
}
static state_t util_detect_init(void *p)
{
	last_dom_util[DOMAIN_NODE] = 1;
	last_dom_util[DOMAIN_CPU] = pc_topology_info.cpu_count;
	last_dom_util[DOMAIN_DRAM] = pc_topology_info.socket_count;
	#if USE_GPUS
	last_dom_util[DOMAIN_GPU]=0;
  if (gpu_data_alloc(&gpu_detection_raw_data)!=EAR_SUCCESS){
    error("Initializing GPU data in powercap");
  }
	#endif
  return EAR_SUCCESS;
}

void util_monitoring_init()
{
	#if USE_GPUS
  sus_util_detection=suscription();
  sus_util_detection->call_main = util_detect_main;
  sus_util_detection->call_init = util_detect_init;
  sus_util_detection->time_relax = 1000;
  sus_util_detection->time_burst = 1000;
  sus_util_detection->suscribe(sus_util_detection);
	#endif
}

/* This function will load any plugin , detect components etc . It must me executed just once */
state_t pmgt_init()
{
	state_t ret;
	int i;
	char basic_path[SZ_PATH_INCOMPLETE];
	topology_init(&pc_topology_info);
	#if USE_GPUS
  if (gpu_init(&gpu_pc)!=EAR_SUCCESS){
    error("Initializing GPU in powercap");
  }
  if (gpu_count(&gpu_pc,&gpu_pc_num_gpus)!=EAR_SUCCESS){
    error("Getting num gpus");
  }
	gpu_pc_model = gpu_model();
	if (gpu_pc_model == MODEL_DUMMY) {
	  pdomains_def_nogpus[DOMAIN_GPUS] = 0.0;
    pdomains_def_withgpus[DOMAIN_GPUS] = 0.0;
		pdomains[DOMAIN_GPUS] = 0.0;
		pdomains_idle[DOMAIN_GPUS] = 0.0;
		debug("Setting the percentage of powercap to GPUS to 0 because DUMMY");
	}
	#endif
	/* DOMAIN_NODE is not cmpatible with CPU+DRAM */
	/* DOMAIN_NODE */
	char *obj_path = getenv("EAR_POWERCAP_POLICY_NODE");
  if (obj_path==NULL){
		/* Plugin per domain node defined */
		if (strcmp(DEFAULT_PC_PLUGIN_NAME_NODE,"noplugin")){
    	sprintf(basic_path,"%s/powercap/%s.so",my_cluster_conf.install.dir_plug,DEFAULT_PC_PLUGIN_NAME_NODE);
    	obj_path=basic_path;
		}
	}
	if (obj_path!=NULL){
		debug("Loading %s powercap plugin domain node",obj_path);
		ret=symplug_open(obj_path, (void **)&pcsyms_fun[DOMAIN_NODE], pcsyms_names, pcsyms_n);
		if (ret==EAR_SUCCESS) domains_loaded[DOMAIN_NODE]=1;
	}else{
		debug("DOMAIN_NODE not loaded");
	}
	/* We have a single domain node */
	debug("Initialzing Node util");
	current_util[DOMAIN_NODE]=calloc(1,sizeof(ulong));
	if (current_util[DOMAIN_NODE]!=NULL) current_util[DOMAIN_NODE][0]=100;
	prev_util[DOMAIN_NODE]=calloc(1,sizeof(ulong));
	if (prev_util[DOMAIN_NODE]!=NULL) prev_util[DOMAIN_NODE][0]=100;
	debug("Static NODE utilization set to 100");
	if (!domains_loaded[DOMAIN_NODE]){
	/* DOMAIN_CPU */
  obj_path = getenv("EAR_POWERCAP_POLICY_CPU");
  if (obj_path==NULL){
    /* Plugin per domain node defined */
    if (strcmp(DEFAULT_PC_PLUGIN_NAME_CPU,"noplugin")){
      sprintf(basic_path,"%s/powercap/%s.so",my_cluster_conf.install.dir_plug,DEFAULT_PC_PLUGIN_NAME_CPU);
      obj_path=basic_path;
    }
  } 
  if (obj_path!=NULL){
    debug("Loading %s powercap plugin domain cpu",obj_path);
    ret=symplug_open(obj_path, (void **)&pcsyms_fun[DOMAIN_CPU], pcsyms_names, pcsyms_n);
    if (ret==EAR_SUCCESS) domains_loaded[DOMAIN_CPU]=1;
  }else{
		debug("DOMAIN_CPU not loaded");
	}
	debug("Initialzing CPU util");
	current_util[DOMAIN_CPU]=calloc(pc_topology_info.cpu_count,sizeof(ulong));
	for (i=0;i<pc_topology_info.cpu_count;i++) current_util[DOMAIN_CPU][i]=100;
	prev_util[DOMAIN_CPU]=calloc(pc_topology_info.cpu_count,sizeof(ulong));
	for (i=0;i<pc_topology_info.cpu_count;i++) prev_util[DOMAIN_CPU][i]=100;
	debug("Static CPU utilization set to 100 for %d cpus",pc_topology_info.cpu_count);

	/* DOMAIN_DRAM */
  obj_path = getenv("EAR_POWERCAP_POLICY_DRAM");
  if (obj_path==NULL){
    /* Plugin per domain node defined */
    if (strcmp(DEFAULT_PC_PLUGIN_NAME_DRAM,"noplugin")){
      sprintf(basic_path,"%s/powercap/%s.so",my_cluster_conf.install.dir_plug,DEFAULT_PC_PLUGIN_NAME_DRAM);
      obj_path=basic_path;
    }
  } 
  if (obj_path!=NULL){
    debug("Loading %s powercap plugin domain dram",obj_path);
    ret=symplug_open(obj_path, (void **)&pcsyms_fun[DOMAIN_DRAM], pcsyms_names, pcsyms_n);
    if (ret==EAR_SUCCESS) domains_loaded[DOMAIN_DRAM]=1;
  }else{
		debug("DOMAIN_DRAM not loaded");
	}
	}
	debug("Initialzing DRAM util");
	current_util[DOMAIN_DRAM]=calloc(pc_topology_info.socket_count,sizeof(ulong));
	for (i=0;i<pc_topology_info.socket_count;i++) current_util[DOMAIN_DRAM][i]=100;
	prev_util[DOMAIN_DRAM]=calloc(pc_topology_info.socket_count,sizeof(ulong));
	for (i=0;i<pc_topology_info.socket_count;i++) prev_util[DOMAIN_DRAM][i]=100;
	debug("Static DRAM utilization set to 100 for %d sockets",pc_topology_info.socket_count);

	#if USE_GPUS
	/* DOMAIN_GPU */
  obj_path = getenv("EAR_POWERCAP_POLICY_GPU");
  if (obj_path==NULL){
    /* Plugin per domain node defined */
    if (strcmp(DEFAULT_PC_PLUGIN_NAME_GPU,"noplugin")){
      sprintf(basic_path,"%s/powercap/%s.so",my_cluster_conf.install.dir_plug,DEFAULT_PC_PLUGIN_NAME_GPU);
      obj_path=basic_path;
    }
  } 
  if (obj_path!=NULL){
    debug("Loading %s powercap plugin domain gpu",obj_path);
    ret=symplug_open(obj_path, (void **)&pcsyms_fun[DOMAIN_GPU], pcsyms_names, pcsyms_n);
    if (ret==EAR_SUCCESS) domains_loaded[DOMAIN_GPU]=1;
		else debug("Error loading GPU powercap plugin");
  }else{
		debug("DOMAIN_GPU not loaded");
		pdomains_def_nogpus[DOMAIN_GPUS] = 0.0;
		pdomains_def_withgpus[DOMAIN_GPUS] = 0.0;
	}
	debug("Initialzing GPU util");
	if (domains_loaded[DOMAIN_GPU]){
		current_util[DOMAIN_GPU]=calloc(gpu_pc_num_gpus,sizeof(ulong));
		for (i=0;i<gpu_pc_num_gpus;i++) current_util[DOMAIN_GPUS][i]=0;
		prev_util[DOMAIN_GPU]=calloc(gpu_pc_num_gpus,sizeof(ulong));
		for (i=0;i<gpu_pc_num_gpus;i++) prev_util[DOMAIN_GPUS][i]=0;
		debug("Static GPU utilization set to 100 for %d GPUS",gpu_pc_num_gpus);
	}
	if (domains_loaded[DOMAIN_NODE]  || domains_loaded[DOMAIN_CPU] || domains_loaded[DOMAIN_DRAM] || domains_loaded[DOMAIN_GPU]) ret=EAR_SUCCESS;
	else ret=EAR_ERROR;
	#else
	if (domains_loaded[DOMAIN_NODE]  || domains_loaded[DOMAIN_CPU] || domains_loaded[DOMAIN_DRAM]) ret=EAR_SUCCESS;
	else ret=EAR_ERROR;
	#endif
	debug("Initializing Util monitoring");
	util_monitoring_init();
	return ret;
}
state_t pmgt_enable(pwr_mgt_t *phandler)
{
	state_t ret,gret=EAR_SUCCESS;
	int i;
  for (i=0;i<NUM_DOMAINS;i++){
    if (pcsyms_fun[i].set_verb_channel!=NULL) pcsyms_fun[i].set_verb_channel(verb_channel);
  }
	for (i=0;i<NUM_DOMAINS;i++){
		debug("Enabled domain %i , loaded %d",i,domains_loaded[i]);
		if (domains_loaded[i]){
			sus[i]=suscription();
			debug("Enabling domain %d",i);
			ret=freturn(pcsyms_fun[i].enable,sus[i]);
			if ((ret!=EAR_SUCCESS) && (domains_loaded[i])) gret=ret;
		}
	}
	if (gret!=EAR_SUCCESS) return gret;
	return gret;
}
state_t pmgt_disable(pwr_mgt_t *phandler)
{
	state_t ret;
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		ret=freturn(pcsyms_fun[i].disable);
	}
	return ret;
}
state_t pmgt_handler_alloc(pwr_mgt_t **phandler)
{
	*phandler=(pwr_mgt_t*)calloc(1,sizeof(pwr_mgt_t *));
	if (*phandler!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}

state_t pmgt_set_powercap_value(pwr_mgt_t *phandler,uint pid,uint domain,ulong limit)
{
	state_t ret,gret=EAR_SUCCESS;
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		debug("Using %f weigth for domain %d: Total %lu allocated %f",pdomains[i],i,limit,limit*pdomains[i]);
		ret=freturn(pcsyms_fun[i].set_powercap_value,pid,domain,limit*pdomains[i],current_util[i]);
		dyn_conf->pc_opt.pper_domain[i]=limit*pdomains[i];
		if ((ret!=EAR_SUCCESS) && (domains_loaded[i])) gret=ret;
	}
	pmgt_limit=limit;
	return gret;
}

static void reallocate_power_between_domains()
{
	int i;
	uint pid=0, domain=0; // we must manage this
	state_t ret;
	for (i=0;i<NUM_DOMAINS;i++){
		debug("Power reallocation: Using %f weigth for domain %d: Total %u allocated %f",pdomains[i],i,pmgt_limit,pmgt_limit*pdomains[i]);
		ret=freturn(pcsyms_fun[i].set_powercap_value,pid,domain,pmgt_limit*pdomains[i],current_util[i]);
	}
}
state_t pmgt_get_powercap_value(pwr_mgt_t *phandler,uint pid,ulong *powercap)
{
	state_t ret,gret;
	ulong total=0,parc;
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		ret=freturn(pcsyms_fun[i].get_powercap_value,pid,&parc);
		if (domains_loaded[i]) total+=parc;
		if ((ret!=EAR_SUCCESS) && (domains_loaded[i])) gret=ret;
	}
	*powercap=total;
	return gret;
}
uint pmgt_is_powercap_enabled(pwr_mgt_t *phandler,uint pid)
{
	uint ret=0,gret=0;
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		ret=freturn(pcsyms_fun[i].is_powercap_policy_enabled,pid);
		if (domains_loaded[i]) gret+=ret;
	}
	return gret;
}
void pmgt_print_powercap_value(pwr_mgt_t *phandler,int fd)
{
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		if (domains_loaded[i]) freturn(pcsyms_fun[i].print_powercap_value,fd);
	}
}
void pmgt_powercap_to_str(pwr_mgt_t *phandler,char *b)
{
	int i;
	char new_s[512];
	for (i=0;i<NUM_DOMAINS;i++){
		if (domains_loaded[i]){ 
			freturn(pcsyms_fun[i].powercap_to_str,new_s);
			strcat(b,new_s);
		}
	}
}

void pmgt_set_status(pwr_mgt_t *phandler,uint status)
{
	debug("pmgt_set_status");
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		if (domains_loaded[i]) freturn(pcsyms_fun[i].set_status,status);
	}
}
uint pmgt_get_powercap_strategy(pwr_mgt_t *phandler)
{
	uint ret,gret=PC_POWER;
	int i;
	debug("pmgt_strategy");
	for (i=0;i<NUM_DOMAINS;i++){
		if (domains_loaded[i]){
			ret=freturn(pcsyms_fun[i].get_powercap_strategy);
			if (ret==PC_DVFS) gret=ret;	
		}
	}
	return gret;
}
void pmgt_set_pc_mode(pwr_mgt_t *phandler,uint mode)
{
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		freturn(pcsyms_fun[i].set_pc_mode,mode);
	}
}

void pmgt_set_power_per_domain(pwr_mgt_t *phandler,dom_power_t *pdomain,uint st)
{
	float pnode,pcpu,pdram,pgpu;
	memcpy(&pdist_per_domain,pdomain,sizeof(dom_power_t));
	pnode=(float)(pdomain->platform-pdomain->gpu)/(float)pdomain->platform;
	pcpu=(float)(pdomain->cpu)/(float)pdomain->platform;
	pdram=(float)(pdomain->dram)/(float)pdomain->platform;
	pgpu=(float)(pdomain->gpu)/(float)pdomain->platform;
	verbose(1,"[NODE-GPU %f CPU %f DRAM %f GPU %f]",pnode,pcpu,pdram,pgpu);

	switch(st){
	case PC_STATUS_IDLE:
		pdomains_idle[DOMAIN_NODE]=pnode;
		pdomains_idle[DOMAIN_CPU]=pcpu;
		pdomains_idle[DOMAIN_DRAM]=pdram;
		pdomains_idle[DOMAIN_GPU]=pgpu;
		break;
	case PC_STATUS_RUN:
		pdomains[DOMAIN_NODE]=pnode;
		pdomains[DOMAIN_CPU]=pcpu;
		pdomains[DOMAIN_DRAM]=pdram;
		pdomains[DOMAIN_GPU]=pgpu;
		break;
	}
	if (st == PC_STATUS_RUN){
		pmgt_powercap_status_per_domain();
	}
	//reallocate_power_between_domains();
}

void pmgt_set_app_req_freq(pwr_mgt_t *phandler,pc_app_info_t *pc_app)
{
	int i;
	if (pcsyms_fun[DOMAIN_NODE].set_app_req_freq != NULL){
  	freturn(pcsyms_fun[DOMAIN_NODE].set_app_req_freq,&pc_app->req_f);
	}
	if (pcsyms_fun[DOMAIN_CPU].set_app_req_freq != NULL){
  	freturn(pcsyms_fun[DOMAIN_CPU].set_app_req_freq,&pc_app->req_f);
	}
	if (pcsyms_fun[DOMAIN_GPU].set_app_req_freq != NULL) {
  	freturn(pcsyms_fun[DOMAIN_GPU].set_app_req_freq,pc_app->req_gpu_f);
	}
}

/* Power needs some reallocation internally in components */
void pmgt_powercap_node_reallocation()
{
	int i;

	for (i=0; i< NUM_DOMAINS;i++){
		if (dom_changed[i]){
			debug("%sDomain %d has changed its utilization %s",COL_RED,i,COL_CLR);
  		freturn(pcsyms_fun[i].set_new_utilization,current_util[i]);
			dom_changed[i] = 0;
		}
	}
	
}

void pmgt_new_job(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_def_nogpus,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
}

void pmgt_end_job(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_def_nogpus,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
}
void pmgt_idle_to_run(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_def_nogpus,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
}
void pmgt_run_to_idle(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_idle,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
}

void pmgt_powercap_status_per_domain()
{
	int i;
	uint ret;
	uint intarget,tbr;
	for (i=0; i< NUM_DOMAINS;i++){
		if (pcsyms_fun[i].get_powercap_status != NULL){
			ret=pcsyms_fun[i].get_powercap_status(&intarget,&tbr);
			if (ret){
				debug("Domain %d is ok with the power: in_target %u, power TBR %u",i,intarget,tbr);
			}else{
				debug("Domain %d cannot share power because is not in the target",i);
			}
		}
	}
}
