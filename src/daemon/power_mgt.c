/**
 * Copyright © 2017-present BSC-Lenovo
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
#include <common/states.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/symplug.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/power_mgt.h>


typedef struct powercap_symbols {
  state_t (*enable)        ();
  state_t (*disable)       ();
  state_t (*set_powercap_value)(uint pid,uint domain,uint limit);
  state_t (*get_powercap_value)(uint pid,uint *powercap);
  uint    (*is_powercap_policy_enabled)(uint pid);
  void    (*set_status)(uint status);
	void		(*set_pc_mode)(uint mode);
  uint    (*get_powercap_strategy)();
  void    (*powercap_to_str)(char *b);
	void 		(*print_powercap_value)(int fd);
	void 		(*set_app_req_freq)(ulong f);
	void 		(*set_verb_channel)(int fd);	
} powercapsym_t;

static dom_power_t pdist_per_domain;
static float pdomains[NUM_DOMAINS]={1.0,0.85,0,0.1};
static uint domains_loaded[NUM_DOMAINS]={0,0,0,0};
static powercapsym_t pcsyms_fun[NUM_DOMAINS];
static void *pcsyms_obj[NUM_DOMAINS] ={ NULL,NULL,NULL,NULL};
const int   pcsyms_n = 12;
extern cluster_conf_t my_cluster_conf;

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
	"set_verb_channel"
};




#define freturn(call, ...) ((call==NULL)?EAR_UNDEFINED:call(__VA_ARGS__));

#define DEFAULT_PC_PLUGIN_NAME_NODE "noplugin"
#define DEFAULT_PC_PLUGIN_NAME_CPU  "dvfs"
#define DEFAULT_PC_PLUGIN_NAME_DRAM "noplugin"
#define DEFAULT_PC_PLUGIN_NAME_GPU  "gpu_dummy"
static uint pc_plugin_loaded=0;

/* This function will load any plugin , detect components etc . It must me executed just once */
state_t pmgt_init()
{
	state_t ret;
	char basic_path[SZ_PATH_INCOMPLETE];
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
  }else{
		debug("DOMAIN_GPU not loaded");
	}
	if (domains_loaded[DOMAIN_NODE]  || domains_loaded[DOMAIN_CPU] || domains_loaded[DOMAIN_DRAM] || domains_loaded[DOMAIN_GPU]) ret=EAR_SUCCESS;
	else ret=EAR_ERROR;
	return ret;
}
state_t pmgt_enable(pwr_mgt_t *phandler)
{
	state_t ret,gret=EAR_SUCCESS;
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		ret=freturn(pcsyms_fun[i].enable);
		if ((ret!=EAR_SUCCESS) && (domains_loaded[i])) gret=ret;
	}
	if (gret!=EAR_SUCCESS) return gret;
  for (i=0;i<NUM_DOMAINS;i++){
    freturn(pcsyms_fun[i].set_verb_channel,verb_channel);
  }
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

state_t pmgt_set_powercap_value(pwr_mgt_t *phandler,uint pid,uint domain,uint limit)
{
	state_t ret,gret=EAR_SUCCESS;
	int i;
	for (i=0;i<NUM_DOMAINS;i++){
		ret=freturn(pcsyms_fun[i].set_powercap_value,pid,domain,limit*pdomains[i]);
		if ((ret!=EAR_SUCCESS) && (domains_loaded[i])) gret=ret;
	}
	return gret;
}
state_t pmgt_get_powercap_value(pwr_mgt_t *phandler,uint pid,uint *powercap)
{
	state_t ret,gret;
	uint total=0,parc;
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

void pmgt_set_power_per_domain(pwr_mgt_t *phandler,dom_power_t *pdomain)
{
	float pnode,pcpu,pdram,pgpu;
	memcpy(&pdist_per_domain,pdomain,sizeof(dom_power_t));
	pnode=(float)(pdomain->platform-pdomain->gpu)/(float)pdomain->platform;
	pcpu=(float)(pdomain->cpu)/(float)pdomain->platform;
	pdram=(float)(pdomain->dram)/(float)pdomain->platform;
	pgpu=(float)(pdomain->gpu)/(float)pdomain->platform;
	verbose(1,"[NODE %f CPU %f DRAM %f GPU %f]",pnode,pcpu,pdram,pgpu);
}

void pmgt_set_app_req_freq(pwr_mgt_t *phandler,ulong f)
{
	int i;
  for (i=0;i<NUM_DOMAINS;i++){
    freturn(pcsyms_fun[i].set_app_req_freq,f);
  }
}

