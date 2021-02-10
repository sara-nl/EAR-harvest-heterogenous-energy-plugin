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
#define SHOW_DEBUGS 1
#include <pthread.h>
#include <common/config.h>
#include <common/config/config_install.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/system/loadavg.h>
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
#ifndef SYN_TEST
extern settings_conf_t *dyn_conf;
extern cluster_conf_t my_cluster_conf;
#else
cluster_conf_t my_cluster_conf;
#endif



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

typedef struct domain_status{
	uint ok;
	uint exceed;
}domain_status_t;

static dom_power_t pdist_per_domain;
static uint pmgt_limit;
static float pdomains[NUM_DOMAINS]={0.6,0.45,0,GPU_PERC_UTIL};
static float pdomains_idle[NUM_DOMAINS]={0.6,0.45,0,GPU_PERC_UTIL};
static domain_status_t cdomain_status[NUM_DOMAINS];
static ulong *current_util[NUM_DOMAINS],*prev_util[NUM_DOMAINS];
static ulong  dom_util[NUM_DOMAINS],last_dom_util[NUM_DOMAINS],dom_changed[NUM_DOMAINS];
static ulong  dom_util_limit[NUM_DOMAINS];
#if USE_GPUS
static float pdomains_def[NUM_DOMAINS]={0.6,0.5,0,GPU_PERC_UTIL};
static float pdomains_def_dummy[NUM_DOMAINS]={1.0,0.9,0,0};
#else
static float pdomains_def[NUM_DOMAINS]={1.0,0.9,0,0};
#endif
static uint domains_loaded[NUM_DOMAINS]={0,0,0,0};
static powercapsym_t pcsyms_fun[NUM_DOMAINS];
static void *pcsyms_obj[NUM_DOMAINS] ={ NULL,NULL,NULL,NULL};
const int   pcsyms_n = 14;

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

#define UTIL_BURST_PERIOD 60000
#define UTIL_RELAX_PERIOD 60000



#define freturn(call, ...) ((call==NULL)?EAR_UNDEFINED:call(__VA_ARGS__));

#define DEFAULT_PC_PLUGIN_NAME_NODE "noplugin"
#define DEFAULT_PC_PLUGIN_NAME_CPU  "noplugin"
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

#define MAX_ALLOWED_DIFF 0.25
#define TDP_CPU 150
#define TDP_DRAM 30
#define TDP_GPU 250
void compute_power_distribution()
{
	int i;
	float node_ratio = 0.1;
	uint pd_total=0,pd_cpu=0,pd_dram=0, pd_gpu=0, pd_pckg;
	uint bpd_cpu,bpd_dram,bpd_gpu = 0;
	bpd_cpu = TDP_CPU;
	bpd_dram = TDP_DRAM;
	#if USE_GPUS
	if ((domains_loaded[DOMAIN_GPU]) && (gpu_pc_model != MODEL_DUMMY)) bpd_gpu = TDP_GPU;
	#endif
	pd_cpu = bpd_cpu * pc_topology_info.socket_count;
	pd_dram = bpd_dram * pc_topology_info.socket_count;
	pd_gpu = bpd_gpu * gpu_pc_num_gpus;
	pd_pckg = pd_cpu + pd_dram;
	pd_total = pd_pckg + pd_gpu + pd_pckg * node_ratio;		
	pdomains_def[DOMAIN_NODE] = (float)(pd_pckg + (float)pd_pckg * node_ratio) / (float)pd_total;
	pdomains_def[DOMAIN_CPU] = (float) pd_pckg / (float)pd_total;
	pdomains_def[DOMAIN_DRAM] = (float) pd_dram / (float)pd_total;
	pdomains_def[DOMAIN_GPUS] = 0;
	#if USE_GPUS
	pdomains_def[DOMAIN_GPUS] = (float) pd_gpu / (float)pd_total;
	#endif
	memcpy(pdomains_idle,pdomains_def,sizeof(pdomains_def));
	debug("W[NODE]=%.2f W[CPU]=%.2f W[GPU]=%.2f",pdomains_def[DOMAIN_NODE],pdomains_def[DOMAIN_CPU],pdomains_def[DOMAIN_GPUS]);
}

/* This function decides if we have to changed the utilization or not */
static uint util_changed(ulong curr,ulong prev)
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
		if (i == DOMAIN_CPU || i == DOMAIN_NODE){
			if (util_changed(dom_util[i],last_dom_util[i])){
				ret=1;
				dom_changed[i]=1;
			}
		}
		#if USE_GPUS
		else if ((i == DOMAIN_GPU ) && (gpu_pc_model != MODEL_DUMMY)){
				for (g=0;g<gpu_pc_num_gpus;g++){
					//debug("Comparing utilization for GPU %d : %lu vs %lu",g,current_util[DOMAIN_GPU][g],prev_util[DOMAIN_GPU][g]);
					if (util_changed(current_util[DOMAIN_GPU][g],prev_util[DOMAIN_GPU][g])){ 
						ret =1;
						dom_changed[i]=1;
					}
				}
		}
		#endif
	}
	return ret;
}

/* These two functions monitors the system utilization : CPU and GPU */
static state_t util_detect_main(void *p)
{
  int i;
	float min,vmin,xvmin,ratio;
	uint runnable,total,lastpid;
	loadavg(&min,&vmin,&xvmin,&runnable,&total,&lastpid);
	debug("%sCPU load %.2f%s ",COL_MGT,min,COL_CLR);
	/* DOMAIN_NODE and DOMAIN_CPU are evaluated as a whole, GPU is evaluated in more detail */
	dom_util[DOMAIN_NODE] = min;
	prev_util[DOMAIN_CPU][0] = current_util[DOMAIN_CPU][0];
	current_util[DOMAIN_CPU][0] = min;
	dom_util[DOMAIN_CPU] = min;
	/* It's pending to define Utilization per CPU */
  for (i=0;i<pc_topology_info.cpu_count;i++){ 
		prev_util[DOMAIN_CPU][i] = current_util[DOMAIN_CPU][i];
		current_util[DOMAIN_CPU][i] = min;
	}

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
	for (i=0;i<NUM_DOMAINS;i++){
		ratio = (float)dom_util[i]/(float)dom_util_limit[i];
		debug("%sDomain %d perc_use %.2f (%.2f/%.1f)%s",COL_RED,i,ratio,(float)dom_util[i],(float)dom_util_limit[i],COL_CLR);
	}
	pmgt_powercap_status_per_domain();
	if (pmgt_utilization_changed()){
		/* Internal reallocation */
    pmgt_powercap_node_reallocation();
  }
	memcpy(last_dom_util,dom_util,sizeof(ulong)*NUM_DOMAINS);
  return EAR_SUCCESS;
}
static state_t util_detect_init(void *p)
{
  float min,vmin,xvmin;
  uint runnable,total,lastpid;
  loadavg(&min,&vmin,&xvmin,&runnable,&total,&lastpid);

	dom_util_limit[DOMAIN_NODE] = pc_topology_info.core_count;
	dom_util_limit[DOMAIN_CPU] = pc_topology_info.core_count;
	dom_util_limit[DOMAIN_DRAM] = pc_topology_info.socket_count ;
	dom_util_limit[DOMAIN_GPU] =  gpu_pc_num_gpus;
	last_dom_util[DOMAIN_NODE] = min;
	last_dom_util[DOMAIN_CPU] = min;
	last_dom_util[DOMAIN_DRAM] = pc_topology_info.socket_count;
	#if USE_GPUS
	last_dom_util[DOMAIN_GPU]=0;
  if (gpu_data_alloc(&gpu_detection_raw_data)!=EAR_SUCCESS){
    error("Initializing GPU data in powercap");
  }
	#endif
  return EAR_SUCCESS;
}

/* This function initilices the utilization monitoring */
void util_monitoring_init()
{
	#if USE_GPUS
  sus_util_detection=suscription();
  sus_util_detection->call_main = util_detect_main;
  sus_util_detection->call_init = util_detect_init;
  sus_util_detection->time_relax = UTIL_RELAX_PERIOD;
  sus_util_detection->time_burst = UTIL_BURST_PERIOD;
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
	if (gpu_load(NULL,0,NULL)!= EAR_SUCCESS){
    error("Initializing GPU(load) in powercap");
	}
  if (gpu_init(&gpu_pc)!=EAR_SUCCESS){
    error("Initializing GPU in powercap");
  }
  if (gpu_count(&gpu_pc,&gpu_pc_num_gpus)!=EAR_SUCCESS){
    error("Getting num gpus");
  }
	gpu_pc_model = gpu_model();
	if (gpu_pc_model == MODEL_DUMMY) {
		memcpy(pdomains_def,pdomains_def_dummy,sizeof(pdomains_def));
		memcpy(pdomains,pdomains_def_dummy,sizeof(pdomains_def));
		memcpy(pdomains_idle,pdomains_def_dummy,sizeof(pdomains_def));
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
    if (ret==EAR_SUCCESS){ 
			domains_loaded[DOMAIN_CPU]=1;
		}else{
			error("Domain CPU not loaded");
		}
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
		memcpy(pdomains_def,pdomains_def_dummy,sizeof(pdomains_def));
		memcpy(pdomains,pdomains_def_dummy,sizeof(pdomains_def));
		memcpy(pdomains_idle,pdomains_def_dummy,sizeof(pdomains_def));
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
	compute_power_distribution();
	memset(cdomain_status,0,NUM_DOMAINS*sizeof(domain_status_t));
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
		//debug("Using %f weigth for domain %d: Total %lu allocated %f",pdomains[i],i,limit,limit*pdomains[i]);
		ret=freturn(pcsyms_fun[i].set_powercap_value,pid,domain,limit*pdomains[i],current_util[i]);
		#ifndef SYN_TEST
		dyn_conf->pc_opt.pper_domain[i]=limit*pdomains[i];
		#endif
		if ((ret!=EAR_SUCCESS) && (domains_loaded[i])) gret=ret;
	}
	pmgt_limit=limit;
	return gret;
}

static void reallocate_power_between_domains()
{
	int i;
	char buffer[1024],buffer2[1024];
	uint pid=0, domain=0; // we must manage this
	state_t ret;
	strcpy(buffer2,"");
	for (i=0;i<NUM_DOMAINS;i++){
		sprintf(buffer,"[DOM=%d W=%.2f Allocated %.1f]",i,pdomains[i],pmgt_limit*pdomains[i]);
		strcat(buffer2,buffer);
		ret=freturn(pcsyms_fun[i].set_powercap_value,pid,domain,pmgt_limit*pdomains[i],current_util[i]);
	}
	debug("%s",buffer2);
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
	#if 0
	float pnode,pcpu,pdram,pgpu;
	memcpy(&pdist_per_domain,pdomain,sizeof(dom_power_t));
	pnode=(float)(pdomain->platform-pdomain->gpu)/(float)pdomain->platform;
	pcpu=(float)(pdomain->cpu+pdomain->dram)/(float)pdomain->platform;
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
	#endif
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
	#if USE_GPUS
	if (pcsyms_fun[DOMAIN_GPU].set_app_req_freq != NULL) {
  	freturn(pcsyms_fun[DOMAIN_GPU].set_app_req_freq,pc_app->req_gpu_f);
	}
	#endif
}

/* Power needs some reallocation internally in components */
void pmgt_powercap_node_reallocation()
{
	int i;
	float ratio;

	for (i=0; i< NUM_DOMAINS;i++){
		ratio = (float)dom_util[i]/(float)dom_util_limit[i];
		debug("%sDomain %d perc_use %.2f (%.2f/%.1f)%s",COL_RED,i,ratio,(float)dom_util[i],(float)dom_util_limit[i],COL_CLR);
		if (dom_changed[i]){
  		freturn(pcsyms_fun[i].set_new_utilization,current_util[i]);
			dom_changed[i] = 0;
		}
	}
	
}

void pmgt_new_job(pwr_mgt_t *phandler)
{
	#if USE_GPUS
	monitor_burst(sus_util_detection);
	#endif
	memcpy(pdomains,pdomains_def,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
}

void pmgt_end_job(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_def,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
	#if USE_GPUS
	monitor_relax(sus_util_detection);
	#endif
}
void pmgt_idle_to_run(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_def,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
	pmgt_set_status(phandler,PC_STATUS_RUN);
}
void pmgt_run_to_idle(pwr_mgt_t *phandler)
{
	memcpy(pdomains,pdomains_idle,sizeof(float)*NUM_DOMAINS);
	reallocate_power_between_domains();
	pmgt_set_status(phandler,PC_STATUS_IDLE);
}

void pmgt_powercap_status_per_domain()
{
	int i;
	uint totalok = 0;
	uint ret,from,to;
	uint intarget,tbr;
	#if USE_GPUS
	memset(cdomain_status,0,sizeof(domain_status_t)*NUM_DOMAINS);
	/* We ask each domain its curent status to distribute power accross domains */
	for (i=0; i< NUM_DOMAINS;i++){
		if (pcsyms_fun[i].get_powercap_status != NULL){
			ret=pcsyms_fun[i].get_powercap_status(&intarget,&tbr);
			if (ret){
				debug("%sDomain %d is ok with the power: in_target %u, power TBR %u%s",COL_GRE,i,intarget,tbr,COL_CLR);
			}
			else{
				debug("Domain %d cannot share power because is not in the target, in_target %u, power TBR %u",i,intarget,tbr);
			}
			cdomain_status[i].ok = intarget;
			cdomain_status[i].exceed = tbr;
			totalok += intarget;
		}
	}
	if ((totalok == 0) || (totalok == 2)) return;
	/* We check first the CPU */
	if (!cdomain_status[DOMAIN_CPU].ok){
		/** Add N watts to the CPU */
		from = DOMAIN_GPU;
		to = DOMAIN_CPU;
	}else{
		from = DOMAIN_CPU;
		to = DOMAIN_GPU;
	}
	uint new_power_recv = pmgt_limit*pdomains[to] + cdomain_status[from].exceed;
	uint new_power_send = pmgt_limit*pdomains[from] - cdomain_status[from].exceed;
	debug("%u Watts have been moved from domain %u to domain %u",cdomain_status[from].exceed,from,to);
	ret=freturn(pcsyms_fun[from].set_powercap_value,0,from,new_power_send,current_util[from]);
	ret=freturn(pcsyms_fun[to].set_powercap_value,0,to,new_power_recv,current_util[to]);

	#endif
	
}
