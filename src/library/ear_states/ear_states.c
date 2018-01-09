/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
extern int report;

// EAR Files
#include <ear_verbose.h>
#include <ear_states/ear_states.h>
#include <ear_models/ear_models.h> 
#include <ear_metrics/ear_papi.h>
#include <ear_metrics/ear_node_energy.h>
#include <ear_frequency/ear_cpufreq.h>
#include <ear_db/ear_db.h>
#include <ear_gui/ear_gui.h>


extern unsigned long EAR_default_frequency;
extern unsigned long ear_frequency;
extern char ear_app_name[MAX_APP_NAME];
extern char ear_node_name[BUFFSIZE];
extern int ear_my_rank;


#define BUFFSIZE 128
unsigned long int eru_init,eru_end,eru_diff;

unsigned int USER_DEFINED_MIN_PERIOD=0;
static struct App_info *MY_SIGNATURE;
static struct App_info MY_LAST_SIGNATURE;
unsigned long POLICY_FREQ;
unsigned int perf_count_period=100;
unsigned int begin_iter,N_iter;

long long comp_N_begin,comp_N_end,comp_N_time;
struct PerfProjection *PP;

double PERF_ACCURACY_MIN_TIME=1000000;

unsigned int EAR_STATE=NO_PERIOD;
int BEGIN_ITER=0;

void states_end_job(int  my_id, FILE *ear_fd,char *app_name)
{
	ear_verbose(1,"EAR(%s) Ends execution. \n",app_name);
}
void states_begin_job(int  my_id, FILE *ear_fd,char *app_name)
{
	char *verbose,*loop_time,*who;
	if (my_id) return;
	PERF_ACCURACY_MIN_TIME=get_ear_performance_accuracy();
	ear_debug(3,"EAR(%s) JOB %s STARTS EXECUTION. Performance accuracy set to (min) %.5lf usecs\n",__FILE__,app_name,PERF_ACCURACY_MIN_TIME);
	EAR_STATE=NO_PERIOD;
	POLICY_FREQ=EAR_default_frequency;
}

int states_my_state()
{
	return EAR_STATE;
}

void states_begin_period(int my_id,FILE *ear_fd,unsigned long event,unsigned int size)
{
	if (my_id==0){
		ear_verbose(4,"EAR(%s): ________BEGIN_PERIOD: Computing N for period %d size %u_____BEGIN_____\n",ear_app_name,event,size);
		EAR_STATE=FIRST_ITERATION;
		db_new_period();
		models_new_period();
		comp_N_begin=metrics_time();
		BEGIN_ITER=0;
		gui_new_period(ear_my_rank,my_id,event);
	}
}

void  states_end_period(int my_id,FILE *ear_fd,unsigned int size,int iterations,unsigned long event)
{
    ear_verbose(4,"EAR(%s)::____________END_PERIOD: END loop detected (Loop ID: %u,size %u)______%d iters______ \n",ear_app_name,event,size,iterations);
}
void states_new_iteration(int my_id,FILE *ear_fd,unsigned int period,int iterations,unsigned long event,unsigned int level)
{
	long long iter_time;
	unsigned long prev_f;
	double CPI,TPI,GBS,POWER,TIME,ENERGY,EDP;
	prev_f=ear_my_frequency();
	switch (EAR_STATE){
	case FIRST_ITERATION:
		if (iterations==1){
			ear_verbose(3,"EAR(%s): FIRST_ITERATION state LoopID %u LoopSize %u\n",ear_app_name,event,period);
			comp_N_end=metrics_time();
			comp_N_time=metrics_usecs_diff(comp_N_end,comp_N_begin);
        		if (comp_N_time<(long long)PERF_ACCURACY_MIN_TIME){// We include a dynamic configurarion of EAR
                		perf_count_period=(PERF_ACCURACY_MIN_TIME/comp_N_time)+1;
			}else{
				perf_count_period=1;
			}
			ear_verbose(3,"\nEAR(%s) at %u,  %d iterations are considered to compute app signature...start computing\n",ear_app_name,prev_f,perf_count_period);
			// Once min iterations is computed for performance accuracy we start computing application signature
			EAR_STATE=EVALUATING_SIGNATURE;
			metrics_start_computing_signature();
			begin_iter=iterations;
			eru_init=read_dc_energy(); // Read DC energy
			ear_debug(3,"EAR(%s) FIRST_ITERATION -> EVALUATING_SIGNATURE\n",ear_app_name);
		}
		break;
	case SIGNATURE_HAS_CHANGED:
		ear_debug(3,"EAR(%s): SIGNATURE_HAS_CHANGED state LoopID %u LoopSize %u iterations %d\n",ear_app_name,event,period,iterations);
        	comp_N_end=metrics_time();
        	comp_N_time=metrics_usecs_diff(comp_N_end,comp_N_begin);
        	if (comp_N_time<(long long)PERF_ACCURACY_MIN_TIME){// We include a dynamic configurarion of DPD behaviour
        		perf_count_period=(PERF_ACCURACY_MIN_TIME/comp_N_time)+1;
        	}else{
        		perf_count_period=1;
        	}
		ear_verbose(3,"EAR(%s) at %u ::(SIGNATURE_HAS_CHANGED)Recomputing N because signature has changed, N set to %d\n",ear_app_name,prev_f,perf_count_period);
        	EAR_STATE=EVALUATING_SIGNATURE;
		ear_debug(3,"EAR(%s) SIGNATURE_HAS_CHANGED -> EVALUATING_SIGNATURE\n",ear_app_name);
		break;
	case RECOMPUTING_N:
		ear_debug(3,"EAR(%s): RECOMPUTING_N state LoopID %u LoopSize %u iterations %d\n",ear_app_name,event,period,iterations);
        	comp_N_end=metrics_time();
        	comp_N_time=metrics_usecs_diff(comp_N_end,comp_N_begin);
		if (comp_N_time==0) ear_verbose(0,"EAR(%s):PANIC comp_N_time must be >0\n",ear_app_name);
        	if (comp_N_time<(long long)PERF_ACCURACY_MIN_TIME){// We include a dynamic configurarion of DPD behaviour
        		perf_count_period=(PERF_ACCURACY_MIN_TIME/comp_N_time)+1;
        	}else{
        		perf_count_period=1;
        	}
		ear_verbose(3,"EAR(%s) at %u ::(RECOMPUTING_N)Recomputing N because F has changed, N set to %d\n",ear_app_name,prev_f,perf_count_period);
		EAR_STATE=SIGNATURE_STABLE;
		ear_debug(3,"EAR(%s) RECOMPUTING_N -> SIGNATURE_STABLE\n",ear_app_name);
		break;
	case EVALUATING_SIGNATURE:
		ear_debug(3,"EAR(%s): EVALUATING_SIGNATURE state LoopID %u LoopSize %u iterations %d\n",ear_app_name,event,period,iterations);
		if ((((iterations-1)%perf_count_period)==0)&&(iterations>1)){
			report=1;
			// GET power consumption for this N iterations
			eru_end=read_dc_energy();
			eru_diff=energy_diff(eru_end , eru_init);
			// END_COMPUTE includes START
			N_iter=iterations-begin_iter;
			MY_SIGNATURE=metrics_end_compute_signature(period,&eru_diff,N_iter,PERF_ACCURACY_MIN_TIME);
			if (MY_SIGNATURE==NULL){
        			comp_N_begin=metrics_time();
                		EAR_STATE=SIGNATURE_HAS_CHANGED;
				ear_debug(3,"EAR(%s) EVALUATING_SIGNATURE -> SIGNATURE_HAS_CHANGED\n",ear_app_name);
			}else{
				CPI=db_get_CPI(MY_SIGNATURE);
				GBS=db_get_GBS(MY_SIGNATURE);
				POWER=db_get_POWER(MY_SIGNATURE);
				TPI=db_get_TPI(MY_SIGNATURE);
				TIME=db_get_seconds(MY_SIGNATURE);
				ENERGY=TIME*POWER;
				EDP=ENERGY*TIME;
				begin_iter=iterations;
				eru_init=eru_end;
				POLICY_FREQ=policy_power(0,MY_SIGNATURE);
				PP=performance_projection(POLICY_FREQ);
				if (POLICY_FREQ!=prev_f){ 
					//metrics_set_signature_start_time();
					comp_N_begin=metrics_time();				
					EAR_STATE=RECOMPUTING_N;
					ear_debug(3,"EAR(%s) EVALUATING_SIGNATURE --> RECOMPUTING_N \n",ear_app_name);
					db_copy_app(&MY_LAST_SIGNATURE,MY_SIGNATURE);
				}else{ 
					EAR_STATE=SIGNATURE_STABLE;
					ear_debug(3,"EAR(%s) EVALUATING_SIGNATURE --> SIGNATURE_STABLE \n",ear_app_name);
				}
	
				gui_new_signature(ear_my_rank,my_id,TIME,CPI,TPI,GBS,POWER);
				gui_frequency(ear_my_rank,my_id,POLICY_FREQ);
				gui_PP(ear_my_rank,my_id,PP->Time,PP->CPI,PP->Power);
				if (POLICY_FREQ!=prev_f){
					ear_verbose(1,"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u,iterations=%d\n\t\t Appplication Signature (CPI=%.3lf GBS=%.3lf Power=%.3lf Time=%.3lf Energy=%.3lfJ EDP=%.3lf)--> New frequency selected %u\n",
					ear_app_name,prev_f,event,period,iterations,CPI,GBS,POWER,TIME,ENERGY,EDP,POLICY_FREQ);
				} else{
			 		ear_verbose(1,"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u-%u,iterations=%d\n\t\t Application Signature (CPI=%.3lf GBS=%.3lf Power=%.3lf Time=%.3lf Energy=%.3lfJ EDP=%.3lf)\n",
					ear_app_name,prev_f,event,period,level,iterations,CPI,GBS,POWER,TIME,ENERGY,EDP);
				}
			}
		}
		break;
	case SIGNATURE_STABLE: 
		ear_debug(3,"EAR(%s): SIGNATURE_STABLE state LoopID=%u, LoopSize=%u,iterations=%d\n",ear_app_name,event,period,iterations);
		// I have executed N iterations more with a new frequency, we must check the signature
		if (((iterations-1)%perf_count_period)==0){
			// GET power consumption for this N iterations
			eru_end=read_dc_energy();
			eru_diff=energy_diff(eru_end , eru_init);
			N_iter=iterations-begin_iter;
			MY_SIGNATURE=metrics_end_compute_signature(period,&eru_diff,N_iter,PERF_ACCURACY_MIN_TIME);
			// returns NULL if time is not enough for performance accuracy
			if (MY_SIGNATURE==NULL){
            			comp_N_begin=metrics_time();
           		 	EAR_STATE=SIGNATURE_HAS_CHANGED;
				ear_verbose(3,"EAR(%s) SIGNATURE_STABLE(NULL) --> SIGNATURE_HAS_CHANGED \n",__FILE__);	
			}else{ 
				CPI=db_get_CPI(MY_SIGNATURE);
				GBS=db_get_GBS(MY_SIGNATURE);
				POWER=db_get_POWER(MY_SIGNATURE);
				TPI=db_get_TPI(MY_SIGNATURE);
				TIME=db_get_seconds(MY_SIGNATURE);
				ENERGY=TIME*POWER;
				EDP=ENERGY*TIME;
				eru_init=eru_end;
				gui_new_signature(ear_my_rank,my_id,TIME,CPI,TPI,GBS,POWER);
				gui_frequency(ear_my_rank,my_id,POLICY_FREQ);
				gui_PP(ear_my_rank,my_id,PP->Time,PP->CPI,PP->Power);
				begin_iter=iterations;
				// We compare the projection with the signature and the old signature
				PP=performance_projection(POLICY_FREQ);
				if (policy_ok(PP,MY_SIGNATURE,&MY_LAST_SIGNATURE)){
					perf_count_period=perf_count_period*2;
					ear_verbose(3,"\n\nEAR(%s): Policy ok Signature (Time %lf Power %lf Energy %lf) Projection(Time %lf Power %lf Energy %lf)\n",
					ear_app_name,TIME,POWER,ENERGY,PP->Time,PP->Power,PP->Time*PP->Power);
					/*
					if (performance_projection_ok(PP,MY_SIGNATURE)==0){
						EAR_STATE=EVALUATING_SIGNATURE;
						ear_debug(3,"EAR(%s) SIGNATURE_STABLE --> EVALUATING_SIGNATURE\n",__FILE__);
					}
					*/
				}else{
				/** If we are not going better **/
					ear_verbose(3,"\n\nEAR(%s): Policy not ok Signature (Time %lf Power %lf Energy %lf) Projection(Time %lf Power %lf Energy %lf)\n",
					ear_app_name,TIME,POWER,ENERGY,PP->Time,PP->Power,PP->Time*PP->Power);
					if (db_signature_has_changed(MY_SIGNATURE,&MY_LAST_SIGNATURE)){
						EAR_STATE=SIGNATURE_HAS_CHANGED;
						ear_verbose(3,"EAR(%s) SIGNATURE_STABLE --> SIGNATURE_HAS_CHANGED \n",ear_app_name);
						comp_N_begin=metrics_time();
						db_new_period();
						models_new_period();
					}else{
						EAR_STATE=EVALUATING_SIGNATURE;
					}	
				}
			}
		}
		break;
	default:
		break;
		ear_verbose(0,"EAR(%s): PANIC:::::STATUS NON EXISTING::::::::::\n",ear_app_name);
		exit(1);
	}
}




