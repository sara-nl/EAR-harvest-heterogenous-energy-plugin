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

#include <ear_db/ear_db.h>
#include <ear_models/ear_models.h>
#include <ear_frequency/ear_cpufreq.h>
#include <ear_verbose.h>
#include <types.h>

#define PERFORMANCE_PENALTY 0.1
// #define PERFORMANCE_GAIN 0.8 , this minimum has been moved to makefile option
#define ACCEPTED_TH 0.05
#define RESET_FREQ 0

extern unsigned long ear_frequency;
extern unsigned long EAR_default_frequency;
extern unsigned int EAR_default_pstate;
extern int ear_use_turbo;
extern int ear_my_rank;
extern int ear_whole_app;
extern char ear_app_name[MAX_APP_NAME];


struct PerfProjection *APP_PERF_PROJECTION;
struct Coefficients_info **COEFFICIENTS_LENOVO;
unsigned int ear_models_pstates=0;
int coeff_fd;
int power_model_policy=MIN_ENERGY_TO_SOLUTION;
double performance_penalty=PERFORMANCE_PENALTY;
double performance_gain=PERFORMANCE_GAIN;
double performance_penalty_th=EAR_ACCEPTED_TH;
unsigned int reset_freq_opt=RESET_FREQ;
char ear_policy_name[MAX_APP_NAME];
double ear_policy_th;
static double T_max;


void init_power_policy()
{
	int min_p_state=0;
	unsigned long def_freq;

	ear_debug(2,"EAR(%s): EAR_init_power_policy\n",__FILE__);
	power_model_policy=get_ear_power_policy();
	switch(power_model_policy){
		case MIN_ENERGY_TO_SOLUTION:
			strcpy(ear_policy_name,"min_energy");
			performance_penalty=get_ear_power_policy_th();
			break;
		case MIN_TIME_TO_SOLUTION:
			strcpy(ear_policy_name,"min_time");
			performance_gain=get_ear_power_policy_th();
			break;
		case MONITORING_ONLY:
			strcpy(ear_policy_name,"monitoring_only");
				break;
	}
	reset_freq_opt=get_ear_reset_freq();
	EAR_default_pstate=get_ear_p_state();
	if (EAR_default_pstate>=ear_get_num_p_states()) EAR_default_pstate=DEFAULT_P_STATE;
	EAR_default_frequency=ear_get_freq(EAR_default_pstate);
	def_freq=db_change_frequency(EAR_default_frequency);
	if (def_freq!=EAR_default_frequency){
		ear_verbose(0,"ear: warning max freq is limited by the system, using %u as default\n",def_freq);
		EAR_default_frequency=def_freq;
	}
	switch(power_model_policy){
		case MIN_ENERGY_TO_SOLUTION: ear_policy_th=performance_penalty_th;break;
		case MIN_TIME_TO_SOLUTION:ear_policy_th=performance_gain;break;
		default:ear_policy_th=0;break;
	}
	ear_verbose(1,"EAR: power policy configuration: policy %s performance penalty %lf accepted th %lf Reset_freq=%u performance gain %lf\n",  (power_model_policy==MONITORING_ONLY)?"MONITORING_ONLY":((power_model_policy==MIN_ENERGY_TO_SOLUTION)?"MIN_ENERGY_TO_SOLUTION":"MIN_TIME_TO_SOLUTION"),reset_freq_opt, performance_penalty,performance_penalty_th,performance_gain);
	ear_verbose(1,"EAR: Default p_state %u Default frequency %lu\n",EAR_default_pstate,EAR_default_frequency);
}
void init_power_models(unsigned int p_states,unsigned long *p_states_list)
{
	char *per_node;
	char *coeff_file_name;
	char coeff_file[128];
	char coeff_default_file[128];
	char coeff_file_fn[128];
	int i,ref;
	int begin_pstate=1,end_pstate;
	struct Coefficients_info coef;
	char nodename[128];
	ear_debug(3,"EAR(%s): EAR_Init_power_models p_states=%u\n",__FILE__,p_states);
	ear_models_pstates=p_states;
	strcpy(coeff_file,get_ear_coeff_db_pathname());
	sprintf(coeff_default_file,"%s.default",coeff_file);
	gethostname(nodename,sizeof(nodename));

	strcat(coeff_file,nodename);
	//sprintf(coeff_file,"%s.%s",coeff_file,nodename);

	ear_verbose(1,"EAR: Using coefficients %s\n",coeff_file);
	COEFFICIENTS_LENOVO=(struct Coefficients_info **)malloc(sizeof(struct Coefficients_info*)*p_states);
	if (COEFFICIENTS_LENOVO==NULL) {
		ear_verbose(0,"EAR: Error allocating memory for p_states coefficients\n");
		exit(1);
	}
	for (i=0;i<p_states;i++){
		COEFFICIENTS_LENOVO[i]=(struct Coefficients_info *)malloc(sizeof(struct Coefficients_info)*p_states);
		if (COEFFICIENTS_LENOVO[i]==NULL) {
			ear_verbose(0,"EAR: Error allocating memory for p_states coefficients fn %d\n",i);
			exit(1);
		}
		for (ref=0;ref<p_states;ref++){
			COEFFICIENTS_LENOVO[i][ref].pstate=p_states_list[ref];
			COEFFICIENTS_LENOVO[i][ref].available=0;
		}
	}
	APP_PERF_PROJECTION=(struct PerfProjection*)malloc(sizeof(struct PerfProjection)*p_states);
	if (APP_PERF_PROJECTION==NULL){
		ear_verbose(0,"EAR: Error allocating memory for perf. projections\n");
		exit(1);
	}
	reset_performance_projection(p_states);
	if (ear_use_turbo) begin_pstate=0;
	end_pstate=p_states;
	for (ref=begin_pstate;ref<end_pstate;ref++){
		sprintf(coeff_file_fn,"%s.%d",coeff_file,p_states_list[ref]);
		ear_verbose(2,"EAR: Opening (per  node) coefficients for %s\n",coeff_file_fn);
		coeff_fd=open(coeff_file_fn,O_RDONLY);
		if (coeff_fd>=0){
			if(read(coeff_fd,COEFFICIENTS_LENOVO[ref],sizeof(struct Coefficients_info)*p_states)!=sizeof(struct Coefficients_info)*p_states){
				ear_verbose(0,"EAR: Error reading coefficienys for F=%u (%s)\n",ref,coeff_file_fn);
				exit(1);
			}
			close(coeff_fd);
		}else{
			// Per node is not available, trying to open default coefficient file
			sprintf(coeff_file_fn,"%s.%d",coeff_default_file,p_states_list[ref]);
			ear_verbose(2,"EAR: Opening (default) coefficients for %s\n",coeff_file_fn);
			coeff_fd=open(coeff_file_fn,O_RDONLY);
			if (coeff_fd>=0){
				if(read(coeff_fd,COEFFICIENTS_LENOVO[ref],sizeof(struct Coefficients_info)*p_states)!=sizeof(struct Coefficients_info)*p_states){
					ear_verbose(0,"EAR: Error reading default coefficients for F=%u (%s)\n",ref,coeff_file_fn);
					exit(1);
				}
				close(coeff_fd);
			}else{
				if (ear_whole_app==0){
					// Coefficients are not available neither for this node neither default
				 	ear_verbose(2,"EAR: Coefficients are not available per node %s and freq %u\n",nodename,p_states_list[ref]);
				}
			}
			
		}
	}
}		

void reset_performance_projection(unsigned int p_states)
{
	ear_debug(4,"EAR(%s) :: ResetperformanceProjection\n",__FILE__);
	int i;
	for (i=0;i<p_states;i++){
		APP_PERF_PROJECTION[i].Time=0;
		APP_PERF_PROJECTION[i].Power=0;	
		APP_PERF_PROJECTION[i].CPI=0;	
	}
}

void set_performance_projection(int i,double TP,double PP,double CPIP)
{
		ear_debug(4,"EAR(%s):: Setting PP for entry %d (%lf,%lf,%lf)\n",__FILE__,i,TP,PP,CPIP);
		APP_PERF_PROJECTION[i].Time=TP;
		APP_PERF_PROJECTION[i].Power=PP;	
		APP_PERF_PROJECTION[i].CPI=CPIP;	
}

double power_projection(struct App_info *my_app,unsigned int Fi)
{
	double POWER_F0,TPI_F0,pp;
	unsigned int Fref;
	ear_debug(4,"EAR(%s)::Computing power projection for nominal %u and f=%u\n",__FILE__,ear_frequency,Fi);
	POWER_F0=NOMINAL_POWER(my_app);
	TPI_F0=NOMINAL_TPI(my_app);
	Fref=ear_get_pstate(ear_frequency);
	pp=COEFFICIENTS_LENOVO[Fref][Fi].A*POWER_F0+COEFFICIENTS_LENOVO[Fref][Fi].B*TPI_F0+COEFFICIENTS_LENOVO[Fref][Fi].C;
	ear_verbose(4,"EAR(%s):: POWER_PROJ=%.3lf=A(%.3lf)*POWERN(%.3lf)+B(%.3lf)*TPIN(%.3lf)+C(%.3lf)\n",
	ear_app_name,
	pp,
	COEFFICIENTS_LENOVO[Fref][Fi].A,
	POWER_F0,
	COEFFICIENTS_LENOVO[Fref][Fi].B,
	TPI_F0,
	COEFFICIENTS_LENOVO[Fref][Fi].C);
	return pp;
}

double cpi_projection(struct App_info *my_app,unsigned int Fi)
{
	double CPI_F0,TPI_F0;
	double cpi_pr;
	unsigned int Fref;
	ear_debug(4,"EAR(%s)::Computing cpi projection for nominal %u and f=%u\n",ear_app_name,ear_frequency,Fi);
	Fref=ear_get_pstate(ear_frequency);
	TPI_F0=NOMINAL_TPI(my_app);
	CPI_F0=NOMINAL_CPI(my_app);
	cpi_pr=COEFFICIENTS_LENOVO[Fref][Fi].F+COEFFICIENTS_LENOVO[Fref][Fi].D*CPI_F0+COEFFICIENTS_LENOVO[Fref][Fi].E*TPI_F0;
	ear_debug(4,"EAR(%s):: CPI_PROJ=%lf=D(%lf)*CPIN(%lf)+E(%lf)*TPIN(%lf)+F(%lf)\n",
	__FILE__,
	cpi_pr,
	COEFFICIENTS_LENOVO[Fref][Fi].D,
	CPI_F0,
	COEFFICIENTS_LENOVO[Fref][Fi].E,
	TPI_F0,
	COEFFICIENTS_LENOVO[Fref][Fi].F);
	return cpi_pr;
}
double time_projection(struct App_info *my_app,unsigned int Fi,double cpi_pr)
{
	double CPI_F0,TIME_F0,timep;
	unsigned int Fref;
	ear_debug(4,"EAR(%s)::Computing time projection for nominal %u and f=%u\n",__FILE__,ear_frequency,Fi);
	Fref=ear_get_pstate(ear_frequency);
	//TIME(fn) = TIME(f0) * CPI(fn)/CPI(f0) * (f0/fn)
	CPI_F0=NOMINAL_CPI(my_app);
	TIME_F0=NOMINAL_TIME(my_app);
	timep=TIME_F0*(cpi_pr/CPI_F0)*((double)ear_frequency/(double)COEFFICIENTS_LENOVO[Fref][Fi].pstate);
	ear_debug(4,"EAR(%s):: TIME_P=%lf= TIMEN(%lf)*(cpi_pr(%lf)/CPI_F0(%lf))*(nominal(%u)/f_proj(%u))\n",
	__FILE__,
	timep,TIME_F0,cpi_pr,CPI_F0,ear_frequency,COEFFICIENTS_LENOVO[Fref][Fi].pstate);
	return timep; 
}

unsigned long optimal_freq_min_energy(double th,struct App_info * SIGNATURE,double *bestPP,double *bestTP)
{
    struct App_info *my_app;
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
    	if (COEFFICIENTS_LENOVO[ref][EAR_default_pstate].available){
    		P_ref=power_projection(my_app,EAR_default_pstate);
    		CPI_ref=cpi_projection(my_app,EAR_default_pstate);
    		T_ref=time_projection(my_app,EAR_default_pstate,CPI_ref);
    		E_ref=P_ref*T_ref;
    		bestSolution=E_ref;
    		bestPstate=EAR_default_frequency;
    	}else{// If we can not project, we use current signature
    		T_ref=my_app->seconds;
    		P_ref=my_app->POWER_f0;
    		CPI_ref=my_app->CPI;
    		E_ref=P_ref*T_ref;
    		bestSolution=E_ref;
    		bestPstate=ear_frequency;
    	}
    }else{ // if we are executing at submission freq
    	T_ref=my_app->seconds;
    	P_ref=my_app->POWER_f0;
    	CPI_ref=my_app->CPI;
    	E_ref=P_ref*T_ref;
    	bestSolution=E_ref;
    	bestPstate=ear_frequency;
    }
	*bestPP=P_ref;*bestTP=T_ref;
    T_max=T_ref+(T_ref*th);
    // MIN_ENERGY_TO_SOLUTION BEGIN
    for (i=min_pstate;i<ear_models_pstates;i++){
        if (COEFFICIENTS_LENOVO[ref][i].available){
            PP=power_projection(my_app,i);
            CPIP=cpi_projection(my_app,i);
            TP=time_projection(my_app,i,CPIP);
            EP=PP*TP;
            if ((EP<bestSolution) && (TP<T_max)){
                    bestPstate=COEFFICIENTS_LENOVO[ref][i].pstate;
                    bestSolution=EP;
					*bestPP=PP;*bestTP=TP;
            }
        }
    }
    // MIN_ENERGY_TO_SOLUTION END
	return 	bestPstate;

}
// This is the main function in this file, it implements power policy
unsigned long policy_power_for_application(unsigned int whole_app,struct App_info * SIGNATURE)
{
	ear_debug(3,"EAR(%s):: EAR_PolicyPower_for_application %u\n",__FILE__,ear_frequency);
	struct App_info *my_app;
	int i,min_pstate;
	unsigned int ref,try_next;
	double freq_gain,perf_gain;
	double PP,TP,CPIP,EP,bestSolution,REF_ENER;
	double P_ref,CPI_ref,T_ref,E_ref,T_current;
	unsigned long bestPstate;
	my_app=SIGNATURE;
	if (ear_use_turbo) min_pstate=0;
	else min_pstate=1;	
	ref=ear_get_pstate(ear_frequency);
	// My reference is the submission frequency
	if (power_model_policy==MONITORING_ONLY){
		bestPstate=ear_frequency;
		T_ref=my_app->seconds;
		P_ref=my_app->POWER_f0;
		CPI_ref=my_app->CPI;
		set_performance_projection(ref,T_ref,P_ref,CPI_ref);
	}else if (power_model_policy==MIN_ENERGY_TO_SOLUTION){
        	if (ear_frequency!=EAR_default_frequency){
                	if (COEFFICIENTS_LENOVO[ref][EAR_default_pstate].available){
                        	P_ref=power_projection(my_app,EAR_default_pstate);
                        	CPI_ref=cpi_projection(my_app,EAR_default_pstate);
                        	T_ref=time_projection(my_app,EAR_default_pstate,CPI_ref);
                        	E_ref=P_ref*T_ref;
                        	bestSolution=E_ref;
                        	bestPstate=EAR_default_frequency;
                	}else{// If we can not project, we use current signature
                        	T_ref=my_app->seconds;
                        	P_ref=my_app->POWER_f0;
                        	CPI_ref=my_app->CPI;
                        	E_ref=P_ref*T_ref;
                        	bestSolution=E_ref;
                        	bestPstate=ear_frequency;
                	}    
        	}else{ // if we are executing at submission freq
                	T_ref=my_app->seconds;
                	P_ref=my_app->POWER_f0;
                	CPI_ref=my_app->CPI;
                	E_ref=P_ref*T_ref;
                	bestSolution=E_ref;
                	bestPstate=ear_frequency;
        	}   
		T_max=T_ref+(T_ref*performance_penalty);
		// MIN_ENERGY_TO_SOLUTION BEGIN
		for (i=min_pstate;i<ear_models_pstates;i++){
		if (COEFFICIENTS_LENOVO[ref][i].available){
			PP=power_projection(my_app,i);
			CPIP=cpi_projection(my_app,i);
			TP=time_projection(my_app,i,CPIP);
			set_performance_projection(i,TP,PP,CPIP);
			EP=PP*TP;
			if (ear_my_rank==0){
				if (ref==EAR_default_pstate){
				ear_debug(4,"EAR POLICY:::: EAR[mpi_rank=%d at %u] REFERENCE_COMPUTED ENERGY(%u) %lf (%lf,%lf) PROJECTED ENERGY(%u) %lf (%lf,%lf) (T_max %lf)\n",
				ear_my_rank,ear_frequency,EAR_default_frequency,E_ref,T_ref,P_ref,COEFFICIENTS_LENOVO[ref][i].pstate,EP,TP,PP,T_max);
				}else{
				ear_debug(4,"EAR POLICY:::: EAR[mpi_rank=%d at %u] REFERENCE_PROJECTED ENERGY(%u) %lf (%lf,%lf) PROJECTED ENERGY(%u) %lf (%lf,%lf) (T_max %lf)\n",
				ear_my_rank,ear_frequency,EAR_default_frequency,E_ref,T_ref,P_ref,COEFFICIENTS_LENOVO[ref][i].pstate,EP,TP,PP,T_max);
				}
			}

			if ((EP<bestSolution) && (TP<T_max)){
					bestPstate=COEFFICIENTS_LENOVO[ref][i].pstate;
					bestSolution=EP;
			}

		}
		}
	// MIN_ENERGY_TO_SOLUTION END
	}else if (power_model_policy==MIN_TIME_TO_SOLUTION){
		// MIN_TIME_TO_SOLUTION BEGIN

                if (ear_frequency!=EAR_default_frequency){
                        if (COEFFICIENTS_LENOVO[ref][EAR_default_pstate].available){
                                P_ref=power_projection(my_app,EAR_default_pstate);
                                CPI_ref=cpi_projection(my_app,EAR_default_pstate);
                                T_ref=time_projection(my_app,EAR_default_pstate,CPI_ref);
                                bestPstate=EAR_default_frequency;
                        }else{// If we can not project, we use current signature
                                T_ref=my_app->seconds;
                                P_ref=my_app->POWER_f0;
                                CPI_ref=my_app->CPI;
                                bestPstate=ear_frequency;
                        }
                }else{ // if we are executing at submission freq
                        T_ref=my_app->seconds;
                        P_ref=my_app->POWER_f0;
                        CPI_ref=my_app->CPI;
                        bestPstate=ear_frequency;
                }

		set_performance_projection(EAR_default_pstate,T_ref,P_ref,CPI_ref);
		if (bestPstate>min_pstate){ // ref=1 is nominal 0=turbo, we are not using it
			try_next=1;
			i=EAR_default_pstate-1;
			T_current=T_ref;
			while(try_next && (i>=min_pstate)){
				if (COEFFICIENTS_LENOVO[ref][i].available){
					PP=power_projection(my_app,i);
					CPIP=cpi_projection(my_app,i);
					TP=time_projection(my_app,i,CPIP);
					set_performance_projection(i,TP,PP,CPIP);
					freq_gain=performance_gain*(double)(COEFFICIENTS_LENOVO[ref][i].pstate-bestPstate)/(double)bestPstate;
					perf_gain=(T_current-TP)/T_current;
					if (perf_gain>=freq_gain){ // OK
						ear_debug(4,"EAR[MIN_TIME][rank %d] go on evaluating other frequencies current %u Tcurrent %lf Tproj %lf perf_gain %lf \n\
						freq_gain %lf (current %u next %u) (cpi actual %lf cpi proj %lf) GBS %lf\n",
						ear_my_rank,bestPstate,T_current,TP,perf_gain,freq_gain,bestPstate,COEFFICIENTS_LENOVO[ref][i].pstate,my_app->CPI,CPIP,my_app->GBS_f0);
						bestPstate=COEFFICIENTS_LENOVO[ref][i].pstate;
						T_current=TP;
						i--;
					}else{
						if ((bestPstate!=ear_frequency)||(bestPstate==EAR_default_frequency)){
						ear_debug(4,"EAR[MIN_TIME][rank %d] Not scaling more than -> Tcurrent[%u]= %lf vs Tproj[%u]= %lf (perf_gain %lf) \n",
						ear_my_rank,bestPstate,T_current,COEFFICIENTS_LENOVO[ref][i].pstate,TP,perf_gain);
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
unsigned int policy_ok(struct PerfProjection *PREDICTION,struct App_info *SIGNATURE,struct App_info *LAST_SIGNATURE)
{
		ear_debug(4,"EAR(%s)::Projection TIME %12.6lf POWER %12.6lf\n",__FILE__,
		PREDICTION->Time,PREDICTION->Power);
		ear_debug(4,"EAR(%s):: Signature Time %12.6lf Power %12.6lf\n",__FILE__,
		SIGNATURE->seconds,SIGNATURE->POWER_f0);
		if (power_model_policy==MIN_TIME_TO_SOLUTION){
			if ((SIGNATURE->seconds>LAST_SIGNATURE->seconds) && (SIGNATURE->f!=LAST_SIGNATURE->f)) return 0;
			if (SIGNATURE->seconds<PREDICTION->Time) return 1;
			else return 0;
		}else if (power_model_policy==MIN_ENERGY_TO_SOLUTION){
			double EP,ER;
			EP=LAST_SIGNATURE->seconds*LAST_SIGNATURE->POWER_f0;
			ER=SIGNATURE->seconds*SIGNATURE->POWER_f0;
			ear_verbose(3,"CURRENT E=%lf (%lf x %lf) LAST E=%lf (%lf x %lf)\n",
				ER,SIGNATURE->seconds,SIGNATURE->POWER_f0,EP,LAST_SIGNATURE->seconds,LAST_SIGNATURE->POWER_f0);
			if ((ER<EP)&&(SIGNATURE->seconds<T_max)) return 1;
			else return 0;
		}else if (power_model_policy==MONITORING_ONLY){
			return 1;
		}	
}
unsigned int performance_projection_ok(struct PerfProjection *PREDICTION,struct App_info *SIGNATURE)
{
	if (equal_with_th(PREDICTION->Time,SIGNATURE->seconds,performance_penalty_th) && equal_with_th(PREDICTION->Power,SIGNATURE->POWER_f0,performance_penalty_th)){
		ear_debug(4,"EAR(%s):: Performance projection OK\n",__FILE__);
		return 1;
	}else return 0;
}
struct PerfProjection * performance_projection(unsigned long f)
{
	ear_debug(4,"EAR(%s):: Getting perfprojection for %u, entry %d\n",__FILE__,f,ear_get_pstate(f));
	return &APP_PERF_PROJECTION[ear_get_pstate(f)];	
}

unsigned long  policy_power(unsigned int whole_app,struct App_info* MY_SIGNATURE)
{
	ear_debug(4,"EAR(%s):: EAR_PolicyPower\n",__FILE__);
	unsigned long optimal_freq=ear_frequency,max_freq;
    optimal_freq=policy_power_for_application(whole_app,MY_SIGNATURE);
	if (optimal_freq!=ear_frequency){
        	ear_debug(3,"EAR(%s):: Changing Frequency to %u at the beggining of iteration\n",__FILE__,optimal_freq);
        	max_freq=db_change_frequency(optimal_freq);
			if (max_freq!=optimal_freq) optimal_freq=max_freq;
	}
	else{
		ear_debug(4,"EAR(%s):: %u selected, no changes are required\n",__FILE__,optimal_freq);
	}
	return optimal_freq;

}

void models_new_period()
{
	ear_debug(4,"EAR(%s):: EAR_models_New_period\n",__FILE__);
	if (reset_freq_opt) db_change_frequency(EAR_default_frequency);
	reset_performance_projection(ear_models_pstates);
}
