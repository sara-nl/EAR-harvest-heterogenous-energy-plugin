/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <library/common/externs.h>
#include <library/ear_models/ear_models.h>
#include <library/ear_frequency/ear_cpufreq.h>
#include <common/types/application.h>
#include <common/ear_verbose.h>
#include <common/states.h>

#define RESET_FREQ 			0
#define PERFORMANCE_PENALTY 0.1
#define ACCEPTED_TH 		0.05
#define NOMINAL_POWER(app)  app->DC_power
#define NOMINAL_TIME(app)   app->time
#define NOMINAL_TPI(app)    app->TPI
#define NOMINAL_CPI(app)    app->CPI

// Extern
int power_model_policy = MIN_ENERGY_TO_SOLUTION;
double performance_penalty = PERFORMANCE_PENALTY;
double performance_gain = PERFORMANCE_GAIN;
double performance_penalty_th = EAR_ACCEPTED_TH;

// Normals
static projection_t *projections;
static coefficient_t **coefficients;
static uint reset_freq_opt = RESET_FREQ;
static uint ear_models_pstates = 0;
static double T_max;
static ulong user_selected_freq;

ulong min_freq(ulong a,ulong b)
{
	if (a<b) return a;
	return b;
}

void print_energy_policy_configuration()
{
    switch(power_model_policy)
    {
        case MIN_ENERGY_TO_SOLUTION:
            strcpy(ear_policy_name,"min_energy");
        break;
        case MIN_TIME_TO_SOLUTION:
            strcpy(ear_policy_name,"min_time");
        break;
        case MONITORING_ONLY:
            strcpy(ear_policy_name,"monitoring_only");
        break;
    }

    ear_verbose(1,"EAR: power policy conf.: policy %s performance penalty %lf performance gain %lf\n",
                ear_policy_name,performance_penalty,performance_gain);
    ear_verbose(1,"EAR: Default p_state %u Default frequency %lu\n", EAR_default_pstate,EAR_default_frequency);

}
// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
void policy_global_reconfiguration()
{
#ifdef SHARED_MEMORY
    // We filter initial configuration
    if (system_conf->max_freq<ear_get_freq(EAR_default_pstate)){
        ear_verbose(0,"EAR max freq set to %lu because of power capping policies \n",system_conf->max_freq);
        EAR_default_pstate=ear_get_pstate(system_conf->max_freq);
    }
    if (performance_gain<system_conf->th){
        ear_verbose(0,"EAR min perf. efficiency th set to %lf because of power capping policies \n",system_conf->th);
        performance_gain=system_conf->th;
    }
	EAR_default_frequency=ear_get_freq(EAR_default_pstate);
#else
	EAR_default_frequency=ear_get_freq(EAR_default_pstate);
#endif
}

// This function returns the pstate corresponding to the maximum frequency taking into account power capping policies
uint get_global_min_pstate()
{
#ifdef SHARED_MEMORY
    ear_get_pstate(system_conf->max_freq);
#else
	return 1;
#endif
}


void init_power_policy()
{
	int min_p_state=0;
	unsigned long def_freq;

	ear_verbose(2,"EAR(%s): EAR_init_power_policy\n",__FILE__);
	
	power_model_policy = get_ear_power_policy();

	if (power_model_policy==MIN_ENERGY_TO_SOLUTION) performance_penalty=get_ear_power_policy_th();
	else if (power_model_policy==MIN_TIME_TO_SOLUTION) performance_gain=get_ear_power_policy_th();	

	reset_freq_opt=get_ear_reset_freq();
	EAR_default_pstate=get_ear_p_state();
	if (EAR_default_pstate>=ear_get_num_p_states()) EAR_default_pstate=DEFAULT_P_STATE;
	user_selected_freq=ear_get_freq(get_ear_p_state());
	policy_global_reconfiguration();

	// TODO: CPUFREQ COUPLED (old db_change_frequency(EAR_default_frequency))
	def_freq = ear_cpufreq_set_node(EAR_default_frequency);

	if (def_freq != EAR_default_frequency)
	{
		ear_verbose(0,"ear: warning max freq is limited by the system, using %u as default\n",
					def_freq);
		EAR_default_frequency = def_freq;
	}
	print_energy_policy_configuration();
}

void init_power_models(unsigned int p_states,unsigned long *p_states_list)
{
	char coeff_file[128];
	char coeff_default_file[128];
	char coeff_file_fn[128];
	char nodename[128];
	int begin_pstate, end_pstate;
	int i, ref;

	ear_debug(3, "EAR(%s): EAR_Init_power_models p_states=%u\n", __FILE__, p_states);

	// Initializations
	begin_pstate = 1;
	ear_models_pstates = p_states;

	// Coefficient file
	strcpy(coeff_file, get_ear_coeff_db_pathname());
	gethostname(nodename, sizeof(nodename));

	// EAR_USER_DB_PATHNAME
	sprintf(coeff_file, "%s%s", coeff_file, nodename);

	ear_verbose(2, "EAR: Using coefficients %s\n", coeff_file);

	// Default coefficient file
	sprintf(coeff_default_file, "%s.default", coeff_file);

	// Coefficient pointers allocation
	coefficients = (coefficient_t **) malloc(sizeof(coefficient_t *) * p_states);

	if (coefficients == NULL) {
		ear_verbose(0, "EAR: Error allocating memory for p_states coefficients\n");
		exit(1);
	}

	for (i = 0; i < p_states; i++)
	{
		coefficients[i] = (coefficient_t *) malloc(sizeof(coefficient_t) * p_states);
		if (coefficients[i] == NULL) {
			ear_verbose(0,"EAR: Error allocating memory for p_states coefficients fn %d\n",i);
			exit(1);
		}

		for (ref = 0; ref < p_states; ref++)
		{
			coefficients[i][ref].pstate = p_states_list[ref];
			coefficients[i][ref].available = 0;
		}
	}

	// Projections allocation
	projections = (projection_t *) malloc(sizeof(projection_t) * p_states);

	if (projections == NULL) {
		ear_verbose(0, "EAR: Error allocating memory for perf. projections\n");
		exit(1);
	}

	//
	reset_performance_projection(p_states);

	// Coefficient pointers allocation and reading
	int size, state;

	if (ear_use_turbo) begin_pstate = 0;
	end_pstate = p_states;

	for (ref = begin_pstate; ref < end_pstate; ref++)
	{
		sprintf(coeff_file_fn, "%s.%d", coeff_file, p_states_list[ref]);

		ear_verbose(2, "EAR: Opening (per node) coefficient file %s\n", coeff_file_fn);

		size = sizeof(coefficient_t) * p_states;
		state = read_coefficients_file(coeff_file_fn, &coefficients[ref], size);

		if (state == EAR_FILE_NOT_FOUND)
		{
			sprintf(coeff_file_fn, "%s.%d", coeff_default_file, p_states_list[ref]);
			ear_verbose(2, "EAR: Opening (default) coefficient file %s\n", coeff_file_fn);

			state = read_coefficients_file(coeff_file_fn, &coefficients[ref], size);
		}

		// If default coefficient file not found
		if (state == EAR_FILE_NOT_FOUND) {
			ear_verbose(2, "EAR: Coefficients are not available per node %s and freq %u\n",
						nodename, p_states_list[ref]);
		}

		// If other king of error during the reading
		if (state == EAR_ALLOC_ERROR || state == EAR_READ_ERROR) {
			ear_verbose(0, "EAR: Error while reading coefficients for %s (%s) (%d)\n",
						coeff_file_fn, state);
			exit(1);
		}
	}
}

void reset_performance_projection(unsigned int p_states)
{
	ear_debug(4,"EAR(%s) :: ResetperformanceProjection\n",__FILE__);
	int i;
	for (i=0;i<p_states;i++){
		projections[i].Time=0;
		projections[i].Power=0;
		projections[i].CPI=0;
	}
}

void set_performance_projection(int i,double TP,double PP,double CPIP)
{
		ear_debug(4,"EAR(%s):: Setting PP for entry %d (%lf,%lf,%lf)\n",__FILE__,i,TP,PP,CPIP);
		projections[i].Time=TP;
		projections[i].Power=PP;
		projections[i].CPI=CPIP;
}

double power_projection(application_t *my_app,unsigned int Fi)
{
	double POWER_F0,TPI_F0,pp;
	unsigned int Fref;
	POWER_F0=NOMINAL_POWER(my_app);
	TPI_F0=NOMINAL_TPI(my_app);
	Fref=ear_get_pstate(ear_frequency);
	pp=coefficients[Fref][Fi].A*POWER_F0+coefficients[Fref][Fi].B*TPI_F0+coefficients[Fref][Fi].C;

	ear_debug(4,"EAR(%s):: POWER_PROJ=%.3lf=A(%.3lf)*POWERN(%.3lf)+B(%.3lf)*TPIN(%.3lf)+C(%.3lf)\n",
	ear_app_name, pp, coefficients[Fref][Fi].A, POWER_F0, coefficients[Fref][Fi].B, TPI_F0, coefficients[Fref][Fi].C);

	return pp;
}

double cpi_projection(application_t *my_app,unsigned int Fi)
{
	double CPI_F0,TPI_F0;
	double cpi_pr;
	unsigned int Fref;
	Fref=ear_get_pstate(ear_frequency);
	TPI_F0=NOMINAL_TPI(my_app);
	CPI_F0=NOMINAL_CPI(my_app);
	cpi_pr=coefficients[Fref][Fi].F+coefficients[Fref][Fi].D*CPI_F0+coefficients[Fref][Fi].E*TPI_F0;

	ear_debug(4,"EAR(%s):: CPI_PROJ=%lf=D(%lf)*CPIN(%lf)+E(%lf)*TPIN(%lf)+F(%lf)\n",
	__FILE__, cpi_pr, coefficients[Fref][Fi].D, CPI_F0, coefficients[Fref][Fi].E, TPI_F0, coefficients[Fref][Fi].F);

	return cpi_pr;
}
double time_projection(application_t *my_app,unsigned int Fi,double cpi_pr)
{
	double CPI_F0,TIME_F0,timep;
	unsigned int Fref;
	Fref=ear_get_pstate(ear_frequency);
	//TIME(fn) = TIME(f0) * CPI(fn)/CPI(f0) * (f0/fn)
	CPI_F0=NOMINAL_CPI(my_app);
	TIME_F0=NOMINAL_TIME(my_app);
	timep=TIME_F0*(cpi_pr/CPI_F0)*((double)ear_frequency/(double)coefficients[Fref][Fi].pstate);

	ear_debug(4,"EAR(%s):: TIME_P=%lf= TIMEN(%lf)*(cpi_pr(%lf)/CPI_F0(%lf))*(nominal(%u)/f_proj(%u))\n",
	__FILE__, timep,TIME_F0,cpi_pr,CPI_F0,ear_frequency,coefficients[Fref][Fi].pstate);

	return timep; 
}

unsigned long optimal_freq_min_energy(double th,application_t * SIGNATURE,double *bestPP,double *bestTP)
{
    application_t *my_app;
    int i,min_pstate;
    unsigned int ref,try_next;
    double freq_gain,perf_gain;
    double PP,TP,CPIP,EP,bestSolution,REF_ENER;
    double P_ref,CPI_ref,T_ref,E_ref,T_max,T_current;
    unsigned long bestPstate;
    my_app=SIGNATURE;
    if (ear_use_turbo) min_pstate=0;
    else min_pstate=1;
    ref=ear_get_pstate(ear_frequency);
    // My reference is the submission frequency
    if (ear_frequency!=EAR_default_frequency){
    	if (coefficients[ref][EAR_default_pstate].available){
    		P_ref=power_projection(my_app,EAR_default_pstate);
    		CPI_ref=cpi_projection(my_app,EAR_default_pstate);
    		T_ref=time_projection(my_app,EAR_default_pstate,CPI_ref);
    		E_ref=P_ref*T_ref;
    		bestSolution=E_ref;
    		bestPstate=EAR_default_frequency;
    	}else{// If we can not project, we use current signature
    		T_ref=my_app->time;
    		P_ref=my_app->DC_power;
    		CPI_ref=my_app->CPI;
    		E_ref=P_ref*T_ref;
    		bestSolution=E_ref;
    		bestPstate=ear_frequency;
    	}
    }else{ // if we are executing at submission freq
    	T_ref=my_app->time;
    	P_ref=my_app->DC_power;
    	CPI_ref=my_app->CPI;
    	E_ref=P_ref*T_ref;
    	bestSolution=E_ref;
    	bestPstate=ear_frequency;
    }
	*bestPP=P_ref;*bestTP=T_ref;
    T_max=T_ref+(T_ref*th);
    // MIN_ENERGY_TO_SOLUTION BEGIN
    for (i=min_pstate;i<ear_models_pstates;i++){
        if (coefficients[ref][i].available){
            PP=power_projection(my_app,i);
            CPIP=cpi_projection(my_app,i);
            TP=time_projection(my_app,i,CPIP);
            EP=PP*TP;
            if ((EP<bestSolution) && (TP<T_max)){
                    bestPstate=coefficients[ref][i].pstate;
                    bestSolution=EP;
					*bestPP=PP;*bestTP=TP;
            }
        }
    }
    // MIN_ENERGY_TO_SOLUTION END
	return 	bestPstate;

}
// This is the main function in this file, it implements power policy
unsigned long policy_power_for_application(unsigned int whole_app,application_t * SIGNATURE)
{
	ear_debug(3,"EAR(%s):: EAR_PolicyPower_for_application %u\n",__FILE__,ear_frequency);
	application_t *my_app;
	int i,min_pstate;
	unsigned int ref,try_next;
	double freq_gain,perf_gain;
	double PP,TP,CPIP,EP,bestSolution,REF_ENER;
	double P_ref,CPI_ref,T_ref,E_ref,T_current;
	unsigned long bestPstate;
	my_app=SIGNATURE;

	if (ear_use_turbo) min_pstate=0;
	else min_pstate=get_global_min_pstate();	

	// This is the frequency at which we were running
	ref=ear_get_pstate(ear_frequency); 

	// This function changes performance_gain,EAR_default_pstate and EAR_default_frequency
	// We must check this is ok changing these values at this point
	policy_global_reconfiguration();

	// My reference is the submission frequency
	if (power_model_policy==MONITORING_ONLY){
		bestPstate=ear_frequency;
		T_ref=my_app->time;
		P_ref=my_app->DC_power;
		CPI_ref=my_app->CPI;
#ifdef SHARED_MEMORY
		if (ear_frequency<system_conf->max_freq){
			// if we are running at a lower frequency than the maximum allowed, we select the 
			// minimum between the maximum and the selected by the user. It receives 2 freqs
			bestPstate=min_freq(system_conf->max_freq,user_selected_freq);
			ear_verbose(0,"EAR monitoring selects new freq based on power capping policies %lu\n",bestPstate);
		}
#endif
		// Anyway this is correct, ref is the previous freq in any case
		set_performance_projection(ref,T_ref,P_ref,CPI_ref);
	}else if (power_model_policy==MIN_ENERGY_TO_SOLUTION){
			// We compute the reference metrics
        	if (ear_frequency!=EAR_default_frequency){
                	if (coefficients[ref][EAR_default_pstate].available){
                        	P_ref=power_projection(my_app,EAR_default_pstate);
                        	CPI_ref=cpi_projection(my_app,EAR_default_pstate);
                        	T_ref=time_projection(my_app,EAR_default_pstate,CPI_ref);
                        	E_ref=P_ref*T_ref;
                        	bestSolution=E_ref;
                        	bestPstate=EAR_default_frequency;
                	}else{// If we can not project, we use current signature
                        	T_ref=my_app->time;
                        	P_ref=my_app->DC_power;
                        	CPI_ref=my_app->CPI;
                        	E_ref=P_ref*T_ref;
                        	bestSolution=E_ref;
                        	bestPstate=ear_frequency;
                	}    
        	}else{ // if we are executing at submission freq
                	T_ref=my_app->time;
                	P_ref=my_app->DC_power;
                	CPI_ref=my_app->CPI;
                	E_ref=P_ref*T_ref;
                	bestSolution=E_ref;
                	bestPstate=ear_frequency;
        	}   
		T_max=T_ref+(T_ref*performance_penalty);
		// MIN_ENERGY_TO_SOLUTION ALGORITHM BEGINS
		for (i=min_pstate;i<ear_models_pstates;i++){
			if (coefficients[ref][i].available){
				PP=power_projection(my_app,i);
				CPIP=cpi_projection(my_app,i);
				TP=time_projection(my_app,i,CPIP);
				set_performance_projection(i,TP,PP,CPIP);
				EP=PP*TP;

				if ((EP<bestSolution) && (TP<T_max)){
					bestPstate=coefficients[ref][i].pstate;
					bestSolution=EP;
				}

			}
		}
	// MIN_ENERGY_TO_SOLUTION ALGORITHM ENDS
	}else if (power_model_policy==MIN_TIME_TO_SOLUTION){
		// MIN_TIME_TO_SOLUTION BEGIN

        if (ear_frequency!=EAR_default_frequency){
                        if (coefficients[ref][EAR_default_pstate].available){
                                P_ref=power_projection(my_app,EAR_default_pstate);
                                CPI_ref=cpi_projection(my_app,EAR_default_pstate);
                                T_ref=time_projection(my_app,EAR_default_pstate,CPI_ref);
                                bestPstate=EAR_default_frequency;
                        }else{// If we can not project, we use current signature
                                T_ref=my_app->time;
                                P_ref=my_app->DC_power;
                                CPI_ref=my_app->CPI;
                                bestPstate=ear_frequency;
                        }
        }else{ // if we are executing at submission freq
                        T_ref=my_app->time;
                        P_ref=my_app->DC_power;
                        CPI_ref=my_app->CPI;
                        bestPstate=ear_frequency;
        }

		set_performance_projection(EAR_default_pstate,T_ref,P_ref,CPI_ref);
		if (bestPstate>min_pstate){ // ref=1 is nominal 0=turbo, we are not using it
			try_next=1;
			i=EAR_default_pstate-1;
			T_current=T_ref;
			while(try_next && (i>=min_pstate)){
				if (coefficients[ref][i].available){
					PP=power_projection(my_app,i);
					CPIP=cpi_projection(my_app,i);
					TP=time_projection(my_app,i,CPIP);
					set_performance_projection(i,TP,PP,CPIP);
					freq_gain=performance_gain*(double)(coefficients[ref][i].pstate-bestPstate)/(double)bestPstate;
					perf_gain=(T_current-TP)/T_current;
					if (perf_gain>=freq_gain){ // OK
						ear_debug(4,"EAR[MIN_TIME][rank %d] go on evaluating other frequencies current %u Tcurrent %lf Tproj %lf perf_gain %lf \n\
						freq_gain %lf (current %u next %u) (cpi actual %lf cpi proj %lf) GBS %lf\n",
						ear_my_rank,bestPstate,T_current,TP,perf_gain,freq_gain,bestPstate,coefficients[ref][i].pstate,my_app->CPI,CPIP,my_app->GBS);
						bestPstate=coefficients[ref][i].pstate;
						T_current=TP;
						i--;
					}else{
						if ((bestPstate!=ear_frequency)||(bestPstate==EAR_default_frequency)){
						ear_debug(4,"EAR[MIN_TIME][rank %d] Not scaling more than -> Tcurrent[%u]= %lf vs Tproj[%u]= %lf (perf_gain %lf) \n",
						ear_my_rank,bestPstate,T_current,coefficients[ref][i].pstate,TP,perf_gain);
						}
						try_next=0;	
					}
				} // Coefficients available
				else try_next=0;
			}	
		}
		// MIN_TIME_TO_SOLUTION ENDS
	}
	// Coefficients were not available for this nominal frequency
#ifdef SHARED_MEMORY
	// Just in case the bestPstate was the frequency at which the application was running
	if (bestPstate>system_conf->max_freq){ 
		ear_verbose(1,"EAR frequency selection updated because of power capping policies (selected %lu --> %lu)\n",
		bestPstate,system_conf->max_freq);
		bestPstate=system_conf->max_freq;
	}
#endif
	return bestPstate;
}

unsigned int equal_with_th(double p,double r,double th)
{
	double diff;
	diff=r-p;
	if (diff<0){// p>r
		diff=diff*-1;
	}
	if (diff<(th*r)) return 1;
	else return 0;
}

uint policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig)
{
	double EP, ER;

	ear_debug(4,"EAR(%s)::Projection TIME %12.6lf POWER %12.6lf\n",
			  __FILE__, proj->Time, proj->Power);
	ear_debug(4,"EAR(%s):: Signature Time %12.6lf Power %12.6lf\n",
			  __FILE__, curr_sig->time, curr_sig->DC_power);

	if (power_model_policy == MIN_TIME_TO_SOLUTION)
	{
		if ((curr_sig->time > last_sig->time) &&
			(curr_sig->avg_f != last_sig->avg_f)) return 0;

		if (curr_sig->time < proj->Time) return 1;
		else return 0;
	}
	else if (power_model_policy == MIN_ENERGY_TO_SOLUTION)
	{
		EP = last_sig->time*last_sig->DC_power;
		ER = curr_sig->time * curr_sig->DC_power;

			ear_verbose(3,"CURRENT E=%lf (%lf x %lf) LAST E=%lf (%lf x %lf)\n",
			ER, curr_sig->time, curr_sig->DC_power, EP, last_sig->time, last_sig->DC_power);

		if ((ER<EP)&&(curr_sig->time<T_max)) return 1;
		else return 0;
	}

	// TODO: else if (power_model_policy==MONITORING_ONLY) return 1;
	return 1;
}

unsigned int performance_projection_ok(projection_t *PREDICTION,application_t *SIGNATURE)
{
	if (equal_with_th(PREDICTION->Time,SIGNATURE->time,performance_penalty_th) && equal_with_th(PREDICTION->Power,SIGNATURE->DC_power,performance_penalty_th)){
		ear_debug(4,"EAR(%s):: Performance projection OK\n",__FILE__);
		return 1;
	}else return 0;
}
projection_t * performance_projection(unsigned long f)
{
	ear_debug(4,"EAR(%s):: Getting perfprojection for %u, entry %d\n",__FILE__,f,ear_get_pstate(f));
	return &projections[ear_get_pstate(f)];
}

unsigned long  policy_power(unsigned int whole_app, application_t* MY_SIGNATURE)
{
	unsigned long optimal_freq, max_freq;

	ear_debug(4,"EAR(%s):: EAR_PolicyPower\n",__FILE__);

	optimal_freq = ear_frequency;
    optimal_freq = policy_power_for_application(whole_app, MY_SIGNATURE);

	if (optimal_freq != ear_frequency)
	{
		ear_debug(3,"EAR(%s):: Changing Frequency to %u at the beggining of iteration\n",
				  __FILE__,optimal_freq);

		// TODO: CPUFREQ COUPLED (old db_change_frequency)
		max_freq = ear_cpufreq_set_node(optimal_freq);

		if (max_freq != optimal_freq) {
			optimal_freq = max_freq;
		}
	}
	else{
		ear_debug(4,"EAR(%s):: %u selected, no changes are required\n",__FILE__,optimal_freq);
	}

	return optimal_freq;

}

void models_new_period()
{
	ear_debug(4,"EAR(%s):: EAR_models_New_period\n", __FILE__);

	if (reset_freq_opt)
	{
		// TODO: CPUFREQ COUPLED (db_change_frequency(EAR_default_frequency))
		ear_cpufreq_set_node(EAR_default_frequency);
	}

	reset_performance_projection(ear_models_pstates);
}
