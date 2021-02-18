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

#if MPI
#include <mpi.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//#define SHOW_DEBUGS 1
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/colors.h>
#include <common/math_operations.h>
#include <common/types/log.h>
#include <common/types/application.h>
#include <library/common/externs.h>
#include <library/common/global_comm.h>
#include <library/metrics/metrics.h>
#include <library/tracer/tracer.h>

extern masters_info_t masters_info;
extern float ratio_PPN;

void state_verbose_signature(loop_t *sig,int master_rank,int show,char *aname,char *nname,int iterations,ulong prevf,ulong new_freq,char *mode)
{
	double CPI, TPI, GBS, POWER, TIME, ENERGY, EDP, VPI, IN_MPI_PERC,IN_MPI_SEC,GPU_POWER;
  float AVGF,AVGGPUF,PFREQ = 0,NFREQ = 0,VI;
	ulong GPU_UTIL,GPU_FREQ;
	int gpui;
	char gpu_buff[256];
	if (master_rank >= 0 || show){
    CPI = sig->signature.CPI; 
    GBS = sig->signature.GBS; 
    POWER = sig->signature.DC_power; 
    TPI = sig->signature.TPI; 
    TIME = sig->signature.time; 
    AVGF = (float)sig->signature.avg_f/1000000.0; 
    VI=metrics_vec_inst(&sig->signature); 
    VPI=(double)VI/(double)sig->signature.instructions; 
    ENERGY = TIME * POWER; 
    EDP = ENERGY * TIME;
	  GPU_POWER=0;GPU_FREQ=0;GPU_UTIL=0; 
		if (prevf > 0 ) PFREQ = (float)prevf/ 1000000.0;
		if (new_freq > 0 ) NFREQ = (float)new_freq/ 1000000.0;
		strcpy(gpu_buff,"");
	  #if USE_GPU_LIB
	  if (sig->signature.gpu_sig.num_gpus>0){ 
		  for(gpui=0;gpui<sig->signature.gpu_sig.num_gpus;gpui++){ 
			  GPU_POWER += sig->signature.gpu_sig.gpu_data[gpui].GPU_power; 
			  GPU_FREQ += sig->signature.gpu_sig.gpu_data[gpui].GPU_freq; 
			  GPU_UTIL += sig->signature.gpu_sig.gpu_data[gpui].GPU_util; 
		  } 
		  AVGGPUF = (float)GPU_FREQ/(float)sig->signature.gpu_sig.num_gpus; 
		  GPU_UTIL = GPU_UTIL/sig->signature.gpu_sig.num_gpus; 
	  } 
	#endif
    verbose(2,"EAR+%s(%s) at %.2f in %s: LoopID=%lu, LoopSize=%lu-%lu,iterations=%d",mode,aname, PFREQ,nname,sig->id.event,sig->id.size,sig->id.level,iterations); 
    verbosen(1,"EAR+%s(%s) at %.2f in %s MR[%d]: ",mode,aname, PFREQ,nname,master_rank); 
    verbosen(1,"%s",COL_GRE);
    if (GPU_UTIL) sprintf(gpu_buff,"(GPU_power %.2lfW GPU_freq %.1fGHz GPU_util %lu)",GPU_POWER,AVGGPUF,GPU_UTIL);
    verbosen(1,"(CPI=%.3lf GBS=%.2lf Power=%.2lfW Time=%.3lfsec.  AVGF=%.2fGHz)\n\t %s", CPI, GBS, POWER, TIME,  AVGF,gpu_buff);
	  if (new_freq > 0) verbosen(1," New Frequency %.2f",NFREQ);
    verbose(1,"%s",COL_CLR);
  }
} 

void state_report_traces(int master_rank, int my_rank,int lid, loop_t *lsig,ulong freq,ulong status)
{
	if (master_rank >= 0 && traces_are_on()){
		traces_new_signature(my_rank, lid, &lsig->signature);
		if (freq > 0 ) traces_frequency(my_rank, lid, freq); 
		traces_policy_state(my_rank, lid,status);
	}
}

void state_report_traces_state(int master_rank, int my_rank,int lid,ulong status)
{
	if (master_rank >= 0 && traces_are_on()) traces_policy_state(my_rank, lid,status);
}
	



/* Auxiliary functions */

void state_print_policy_state(int master_rank,int st)
{
	#if 0
	if (master_rank >= 0){
	switch(st){
		case EAR_POLICY_READY:verbose(2,"Policy new state EAR_POLICY_READY");break;
		case EAR_POLICY_CONTINUE:verbose(2,"Policy new state EAR_POLICY_CONTINUE");break;
		case EAR_POLICY_GLOBAL_EV:verbose(2,"Policy new state EAR_POLICY_GLOBAL_EV");break;
		case EAR_POLICY_GLOBAL_READY: verbose(2,"Policy new state EAR_POLICY_GLOBAL_READY"); break;
	}
	}
	#endif
}


static void debug_loop_signature(char *title, signature_t *loop)
{
  float avg_f = (float) loop->avg_f / 1000000.0;

  debug("(%s) Avg. freq: %.2lf (GHz), CPI/TPI: %0.3lf/%0.3lf, GBs: %0.3lf, DC power: %0.3lf, time: %0.3lf, GFLOPS: %0.3lf",
                title, avg_f, loop->CPI, loop->TPI, loop->GBS, loop->DC_power, loop->time, loop->Gflops);
}

