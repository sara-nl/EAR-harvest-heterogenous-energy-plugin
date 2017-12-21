/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <cpufreq.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <papi.h>

#include <environment.h>
#include <ear_verbose.h>
#include <ear_frequency/ear_cpufreq.h>
#include <ear_metrics/ear_papi.h>
#include <ear_db/ear_db.h>


unsigned int numcpus=0;
int ear_db_historic=-1;

int ear_active_apps=0;

struct App_info *CURRENT_APP;
unsigned long CURRENT_FREQ;
unsigned long ear_app_frequency;
unsigned int ear_id_requested=0;
char ear_app_name[256];
extern char ear_node_name[MAX_APP_NAME];
extern int ear_my_rank;
extern double performance_penalty_th;
extern unsigned long EAR_default_frequency;


// This function initilices application information
void db_init(unsigned int whole_app,char *app_name)
{
	const char *ear_DB_name;
	char ear_db_def[128];
	char ear_db_his[128];
	int i;
	char * job_id,*logname;

	ear_debug(2,"EAR(%s): db_init. LEARNING_PHASE %d\n",__FILE__,whole_app);
	CURRENT_FREQ=ear_cpufreq_get(0);
	// We reserve space for application signature
	CURRENT_APP=(struct App_info*)malloc(sizeof(struct App_info)*ear_get_num_p_states());
	if (CURRENT_APP==NULL){
		ear_verbose(0,"EAR:Error allocating memory for CURRENT_APP\n");	
		exit(1);
	}
	memset(CURRENT_APP,0,sizeof(struct App_info)*ear_get_num_p_states());
	ear_debug(3,"EAR(%s):Allocated memory for %u pstates at %p\n",__FILE__,ear_get_num_p_states(),CURRENT_APP);
	job_id=getenv("SLURM_JOB_ID");
	logname=getenv("LOGNAME");	
	for (i=0;i<ear_get_num_p_states();i++){
		CURRENT_APP[i].nominal=EAR_default_frequency;
		CURRENT_APP[i].f=ear_get_freq(i);
		strcpy(CURRENT_APP[i].app_id,app_name);
		strcpy(CURRENT_APP[i].user_id,logname);
		if (job_id!=NULL) strcpy(CURRENT_APP[i].job_id,job_id);
		else strcpy(CURRENT_APP[i].job_id,"NA");
		strcpy(CURRENT_APP[i].node_id,ear_node_name);
	}	
}
	
void db_end(unsigned int whole_app)
{
	ear_debug(4,"EAR(%s) db_end\n",__FILE__);
}

void db_update_historical(unsigned int whole_app,struct App_info *MY_APP)
{
	// At the end of the execution, we add one line to the historic DB file with the summary of the metrics
	ear_daemon_client_write_app_signature(MY_APP);
    ear_debug(4,"EAR(%s): Update values for app_id %s\n",__FILE__,MY_APP->app_id);
	db_print_app_info(MY_APP);
}

// A=B
void db_copy_app(struct App_info *A,struct App_info *B)
{
	ear_debug(4,"EAR(%s): db_copy_app\n",__FILE__);
    strcpy(A->app_id,B->app_id);
    strcpy(A->job_id,B->job_id);
    strcpy(A->user_id,B->user_id);
    strcpy(A->node_id,B->node_id);
    strcpy(A->policy,B->policy);
	A->th=B->th;
    A->f=B->f;
    A->procs=B->procs;
    A->seconds=B->seconds;
    A->GIPS_f0=B->GIPS_f0;
    A->GBS_f0=B->GBS_f0;
    A->POWER_f0=B->POWER_f0;
    A->TPI_f0=B->TPI_f0;
    A->CPI=B->CPI;
    A->nominal=B->nominal;
    A->CYCLES=B->CYCLES;
    A->INSTRUCTIONS=B->INSTRUCTIONS;
    A->POWER_DC=B->POWER_DC;
    A->DRAM_POWER=B->DRAM_POWER;
    A->PCK_POWER=B->PCK_POWER;
	A->Gflops=B->Gflops;
	A->EDP=B->EDP;
	db_print_app_info(A);
        
}
// We search for app_name in DB and fill CURRENT_APP
void init_current_app(char *app_name)
{
	int i;
	for (i=0;i<ear_get_num_p_states();i++){	
		CURRENT_APP[i].f=0;
		strcpy(CURRENT_APP[i].app_id,app_name);
	}
}

int db_get_app_name(char *my_name)
{
	char *app_name;
	int defined=0;
	app_name=get_ear_app_name();
    if (app_name==NULL){
		if (PAPI_is_initialized()==PAPI_NOT_INITED) strcpy(my_name,"UnknownApplication");
		else metrics_get_app_name(my_name);
		set_ear_app_name(my_name);
    }else{
		defined=1;
		strcpy(my_name,app_name);
	}
	
    ear_verbose(1,"EAR::Application name is  %s\n",my_name);
	return defined;
}


// Sets the specified frequency f in all the cpus in the node (first version)
unsigned long db_change_frequency(unsigned long f)
{
	unsigned int i;
	ear_debug(1,"EAR(%s) Setting node cpufrequency %u\n",__FILE__,f);
	CURRENT_FREQ=ear_cpufreq_set_node(f);	
	return CURRENT_FREQ;
	
}

// Operations required at the begginning of a new period
void db_new_period()
{
	ear_debug(4,"EAR(%s) db_new_period\n",__FILE__);
	
}
unsigned int db_equal_with_th(double a,double b,double th)
{
	int eq;
        if (a>b){
                if (a<(b*(1+th))) eq=1;
                else eq=0;
        }else{
                if ((a*(1+th))>b) eq=1;
                else eq=0;
        }
	if (eq==0){
		ear_debug(4,"Values are diff:a %lf b %lf (a+%lf\%=%lf...b+%lf\%=%lf) \n",a,b,th,a*(1+th),th,b*(1+th)); 
	}
	return eq;
}


int db_signature_has_changed(struct App_info *A,struct App_info *B)
{
	if (db_equal_with_th(A->CPI,B->CPI,performance_penalty_th) && db_equal_with_th(A->GBS_f0,B->GBS_f0,performance_penalty_th))  return 0;
	else return 1;
}
struct App_info * db_current_app()
{
	return &CURRENT_APP[ear_get_pstate(CURRENT_FREQ)];
}
//
// SET FUNCTIONS
//
void db_set_policy(struct App_info *MY_APP,char *policy_name)
{
	strcpy(MY_APP->policy,policy_name);
}
void db_set_th(struct App_info *MY_APP,double th)
{
	MY_APP->th=th;
}
void db_set_frequency(struct App_info *MY_APP,unsigned long f)
{
        MY_APP->f=f;
}
void db_set_default(struct App_info *MY_APP,unsigned long f)
{
	MY_APP->nominal=f;
}

void db_set_procs(struct App_info *MY_APP,unsigned int procs)
{
        MY_APP->procs=procs;
}

void db_set_GBS(struct App_info *MY_APP,double gbs)
{
	MY_APP->GBS_f0=gbs;
}
void db_set_GIPS(struct App_info *MY_APP,double gips)
{
	MY_APP->GIPS_f0=gips;
}
void db_set_POWER(struct App_info *MY_APP,double power)
{
	MY_APP->POWER_f0=power;
}
void db_set_TPI(struct App_info *MY_APP,double tpi)
{
	MY_APP->TPI_f0=tpi;
}

void db_set_seconds(struct App_info *MY_APP,double seconds)
{
	MY_APP->seconds=seconds;
}

void db_set_CPI(struct App_info *MY_APP,double CPI)
{
        MY_APP->CPI=CPI;
}
void db_set_Gflops(struct App_info *MY_APP,double Gflops)
{
	MY_APP->Gflops=Gflops;
}
void db_set_EDP(struct App_info *MY_APP,double EDP)
{
	MY_APP->EDP=EDP;
}
void db_set_CYCLES(struct App_info *MY_APP,long long cycles)
{
        MY_APP->CYCLES=cycles;
}
void db_set_INSTRUCTIONS(struct App_info *MY_APP,long long instr)
{
        MY_APP->INSTRUCTIONS=instr;
}
void db_set_POWER_DC(struct App_info *MY_APP,double power_dc)
{
        MY_APP->POWER_DC=power_dc;
}
void db_set_DRAM_POWER(struct App_info *MY_APP,double dram_power)
{
        MY_APP->DRAM_POWER=dram_power;
}
void db_set_PCK_POWER(struct App_info *MY_APP,double pck_power)
{
        MY_APP->PCK_POWER=pck_power;
}


// GET FUNCTIONS


char * db_get_Name(struct App_info *MY_APP)
{
        return MY_APP->app_id;

}
unsigned long db_get_frequency(struct App_info *MY_APP)
{
        return MY_APP->f;
}

unsigned int  db_get_procs(struct App_info *MY_APP)
{
        return MY_APP->procs;
}

double db_get_Gflops(struct App_info *MY_APP)
{
	return MY_APP->Gflops;
}
double db_get_EDP(struct App_info *MY_APP)
{
	return MY_APP->EDP;
}

double db_get_GBS(struct App_info *MY_APP)
{
	return MY_APP->GBS_f0;
}
double db_get_GIPS(struct App_info *MY_APP)
{
	return MY_APP->GIPS_f0;
}
double db_get_POWER(struct App_info *MY_APP)
{
	return MY_APP->POWER_f0;
}
double db_get_TPI(struct App_info *MY_APP)
{
	return MY_APP->TPI_f0;
}

double db_get_seconds(struct App_info *MY_APP)
{
	return MY_APP->seconds;
}
double db_get_CPI(struct App_info *MY_APP)
{
	return MY_APP->CPI;
}

void db_print_app_info(struct App_info *APP)
{
        ear_debug(2,"\t\t ***DEBUG*** Job_id %s Username %s Nodename %s App_name %s freq %u procs %u GIPS_f0 %.5lf GBS_f0 %.5lf POWER_f0 %.5lf TPI_f0 %12.6lf seconds %12.6lf CPI %.5lf nominal %u CYCLES %llu INSTRUCTIONS %llu DC_NODE_POWER %12.6lf DRAM_POWER %.5lf PCK_POWER %12.6lf\n",
		APP->job_id,
		APP->user_id,
		APP->node_id,
        APP->app_id,
        APP->f,
        APP->procs,
        APP->GIPS_f0,
        APP->GBS_f0,
        APP->POWER_f0,
        APP->TPI_f0,
        APP->seconds,
        APP->CPI,
        APP->nominal,
		APP->CYCLES,
		APP->INSTRUCTIONS,
		APP->POWER_DC,
		APP->DRAM_POWER,
		APP->PCK_POWER);

}
