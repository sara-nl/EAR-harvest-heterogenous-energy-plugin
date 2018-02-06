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

#include <ear_frequency/ear_cpufreq.h>
#include <ear_metrics/ear_papi.h>
#include <ear_db/ear_db.h>
#include <ear_verbose.h>
#include <environment.h>
#include <externs.h>

static application_t *apps;
static ulong current_freq;

// TODO: remove
extern double performance_penalty_th; // (-> ear_models <-)

// This function initilices application information
void db_init(unsigned int whole_app, char *app_name) {
    const char *ear_DB_name;
    char ear_db_def[128];
    char ear_db_his[128];
    int i;
    char *job_id, *logname;

    ear_debug(2, "EAR(%s): db_init. LEARNING_PHASE %d\n", __FILE__, whole_app);
    current_freq = ear_cpufreq_get(0);
    // We reserve space for application signature
    apps = (application_t *) malloc(sizeof(application_t) * ear_get_num_p_states());
    if (apps == NULL) {
        ear_verbose(0, "EAR:Error allocating memory for apps\n");
        exit(1);
    }
    memset(apps, 0, sizeof(application_t) * ear_get_num_p_states());
    ear_debug(3, "EAR(%s):Allocated memory for %u pstates at %p\n", __FILE__, ear_get_num_p_states(), apps);
    job_id = getenv("SLURM_JOB_ID");
    logname = getenv("LOGNAME");
    for (i = 0; i < ear_get_num_p_states(); i++) {
        apps[i].def_f = EAR_default_frequency;
        apps[i].avg_f = ear_get_freq(i);
        strcpy(apps[i].app_id, app_name);
        strcpy(apps[i].user_id, logname);
        if (job_id != NULL) strcpy(apps[i].job_id, job_id);
        else {
            sprintf(apps[i].job_id, "%d", getppid());
        }
        strcpy(apps[i].node_id, ear_node_name);
    }
}

void db_end(unsigned int whole_app) {
    ear_debug(4, "EAR(%s) db_end\n", __FILE__);
}

void db_update_historical(unsigned int whole_app, application_t *MY_APP) {
    // At the end of the execution, we add one line to the historic DB file with the summary of the metrics
    ear_daemon_client_write_app_signature(MY_APP);
    ear_debug(4, "EAR(%s): Update values for app_id %s\n", __FILE__, MY_APP->app_id);
    db_print_app_info(MY_APP);
}

// A=B
void db_copy_app(application_t *A, application_t *B) {
    ear_debug(4, "EAR(%s): db_copy_app\n", __FILE__);
    strcpy(A->app_id, B->app_id);
    strcpy(A->job_id, B->job_id);
    strcpy(A->user_id, B->user_id);
    strcpy(A->node_id, B->node_id);
    strcpy(A->policy, B->policy);
    A->policy_th = B->policy_th;
    A->avg_f = B->avg_f;
    A->procs = B->procs;
    A->iter_time = B->iter_time;
    A->GBS = B->GBS;
    A->DC_power = B->DC_power;
    A->TPI = B->TPI;
    A->CPI = B->CPI;
    A->def_f = B->def_f;
    A->cycles = B->cycles;
    A->instructions = B->instructions;
    A->DRAM_power = B->DRAM_power;
    A->PCK_power = B->PCK_power;
    A->Gflops = B->Gflops;
    A->EDP = B->EDP;
    db_print_app_info(A);

}

// We search for app_name in DB and fill apps
void init_current_app(char *app_name) {
    int i;
    for (i = 0; i < ear_get_num_p_states(); i++) {
        apps[i].avg_f = 0;
        strcpy(apps[i].app_id, app_name);
    }
}

int db_get_app_name(char *my_name) {
    char *app_name;
    int defined = 0;
    app_name = get_ear_app_name();
    if (app_name == NULL) {
        if (PAPI_is_initialized() == PAPI_NOT_INITED) strcpy(my_name, "UnknownApplication");
        else metrics_get_app_name(my_name);
        set_ear_app_name(my_name);
    } else {
        defined = 1;
        strcpy(my_name, app_name);
    }

    ear_verbose(1, "EAR: Application name is  %s\n", my_name);
    return defined;
}


// Sets the specified frequency f in all the cpus in the node (first version)
unsigned long db_change_frequency(unsigned long f) {
    unsigned int i;
    ear_debug(1, "EAR(%s) Setting node cpufrequency %u\n", __FILE__, f);
    current_freq = ear_cpufreq_set_node(f);
    return current_freq;
}

// Operations required at the begginning of a new period
void db_new_period() {
    ear_debug(4, "EAR(%s) db_new_period\n", __FILE__);
}

unsigned int db_equal_with_th(double a, double b, double th) {
    int eq;
    if (a > b) {
        if (a < (b * (1 + th))) eq = 1;
        else eq = 0;
    } else {
        if ((a * (1 + th)) > b) eq = 1;
        else eq = 0;
    }
    if (eq == 0) {
        ear_debug(4, "Values are diff:a %lf b %lf (a+%lf\%=%lf...b+%lf\%=%lf) \n", a, b, th, a * (1 + th), th,
                  b * (1 + th));
    }
    return eq;
}

//TODO: no sense to be here
int db_signature_has_changed(application_t *A, application_t *B)
{
    if (db_equal_with_th(A->CPI, B->CPI, performance_penalty_th) &&
        db_equal_with_th(A->GBS, B->GBS, performance_penalty_th))
    {
        return 0;
    }
    return 1;
}

application_t *db_current_app() {
    return &apps[ear_get_pstate(current_freq)];
}

//
// SET FUNCTIONS
//
void db_set_policy(application_t *MY_APP, char *policy_name) {
    strcpy(MY_APP->policy, policy_name);
}

void db_set_th(application_t *MY_APP, double th) {
    MY_APP->policy_th = th;
}

void db_set_frequency(application_t *MY_APP, unsigned long f) {
    MY_APP->avg_f = f;
}

void db_set_default(application_t *MY_APP, unsigned long f) {
    MY_APP->def_f = f;
}

void db_set_procs(application_t *MY_APP, unsigned int procs) {
    MY_APP->procs = procs;
}

void db_set_GBS(application_t *MY_APP, double gbs) {
    MY_APP->GBS = gbs;
}

void db_set_POWER(application_t *MY_APP, double power) {
    MY_APP->DC_power = power;
}

void db_set_TPI(application_t *MY_APP, double tpi) {
    MY_APP->TPI = tpi;
}

void db_set_seconds(application_t *MY_APP, double seconds) {
    MY_APP->iter_time = seconds;
}

void db_set_CPI(application_t *MY_APP, double CPI) {
    MY_APP->CPI = CPI;
}

void db_set_Gflops(application_t *MY_APP, double Gflops) {
    MY_APP->Gflops = Gflops;
}

void db_set_EDP(application_t *MY_APP, double EDP) {
    MY_APP->EDP = EDP;
}

void db_set_CYCLES(application_t *MY_APP, long long cycles) {
    MY_APP->cycles = cycles;
}

void db_set_INSTRUCTIONS(application_t *MY_APP, long long instr) {
    MY_APP->instructions = instr;
}

void db_set_DRAM_POWER(application_t *MY_APP, double dram_power) {
    MY_APP->DRAM_power = dram_power;
}

void db_set_PCK_POWER(application_t *MY_APP, double pck_power) {
    MY_APP->PCK_power = pck_power;
}

// GET FUNCTIONS
char *db_get_Name(application_t *MY_APP) {
    return MY_APP->app_id;

}

unsigned long db_get_frequency(application_t *MY_APP) {
    return MY_APP->avg_f;
}

unsigned int db_get_procs(application_t *MY_APP) {
    return MY_APP->procs;
}

double db_get_Gflops(application_t *MY_APP) {
    return MY_APP->Gflops;
}

double db_get_EDP(application_t *MY_APP) {
    return MY_APP->EDP;
}

double db_get_GBS(application_t *MY_APP) {
    return MY_APP->GBS;
}

double db_get_POWER(application_t *MY_APP) {
    return MY_APP->DC_power;
}

double db_get_TPI(application_t *MY_APP) {
    return MY_APP->TPI;
}

double db_get_seconds(application_t *MY_APP) {
    return MY_APP->iter_time;
}

double db_get_CPI(application_t *MY_APP) {
    return MY_APP->CPI;
}

void db_print_app_info(application_t *APP)
{
    ear_debug(2, "\t\t ***DEBUG*** Job_id %s Username %s Nodename %s App_name %s freq %u procs %u" \
                 "GBS_f0 %.5lf POWER_f0 %.5lf TPI_f0 %12.6lf seconds %12.6lf CPI %.5lf nominal %u" \
                 "CYCLES %llu INSTRUCTIONS %llu DRAM_POWER %.5lf PCK_POWER %12.6lf\n",
                 APP->job_id, APP->user_id, APP->node_id, APP->app_id, APP->avg_f, APP->procs,
                 APP->GBS, APP->DC_power, APP->TPI, APP->iter_time, APP->CPI, APP->def_f,
                 APP->cycles, APP->instructions, APP->DRAM_power, APP->PCK_power);
}
