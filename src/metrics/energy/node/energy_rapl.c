/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * * 
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * * 
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING  
 * */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <common/states.h>
#include <common/hardware/hardware_info.h>
#include <metrics/energy/node/energy_node.h>
#include <metrics/energy/energy_cpu.h>
#include <common/math_operations.h>

#define SHOW_DEBUGS 1
#ifdef SHOW_DEBUGS
#define debug(...) fprintf(stderr, __VA_ARGS__); 
#else
#define debug(...) 
#endif

static pthread_mutex_t rapl_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * MAIN FUNCTIONS
 */
static int num_pack;
state_t energy_init(void **c)
{
	int ret;
	state_t st;
	int *pfd;
	if (c==NULL) return EAR_ERROR;
	if (detect_packages(&num_pack)!=EAR_SUCCESS) return EAR_ERROR;
	*c=(int  *)malloc(sizeof(int)*num_pack);
	if (*c==NULL) return EAR_ERROR;
	pthread_mutex_lock(&rapl_lock);
	pfd=*c;
	*pfd=init_rapl_msr(pfd);
	pthread_mutex_unlock(&rapl_lock);
	if (*pfd<0)	return EAR_ERROR;
	return EAR_SUCCESS;
}
state_t energy_dispose(void **c)
{
	int *pfd;
	if ((c==NULL) || (*c==NULL)) return EAR_ERROR;
	pthread_mutex_lock(&rapl_lock);
	pfd=*c;
	dispose_rapl_msr(pfd);	
	free(*c);
	pthread_mutex_unlock(&rapl_lock);
	return EAR_SUCCESS;
}
state_t energy_datasize(size_t *size)
{
	*size=sizeof(unsigned long long)*RAPL_POWER_EVS*num_pack;
	return EAR_SUCCESS;
}
state_t energy_frequency(ulong *freq_us)
{
	*freq_us=10000;	
	return EAR_SUCCESS;
}

state_t energy_dc_read(void *c, edata_t energy_mj)
{
	state_t st;
	int *pfd;
  pfd=(int *)c;

	unsigned long long *pvalues=energy_mj;
	memset(pvalues,0,sizeof(unsigned long long)*RAPL_POWER_EVS*num_pack);
	return read_rapl_msr(pfd,pvalues);
}


state_t energy_dc_time_read(void *c, edata_t energy_mj, ulong *time_ms)
{
	state_t st;
	struct timeval t;
	int i;
	int *pfd;
	pfd=(int *)c;

	unsigned long long *pvalues=energy_mj;
	memset(pvalues,0,sizeof(unsigned long long)*RAPL_POWER_EVS*num_pack);
	st=read_rapl_msr(pfd,pvalues);
	*time_ms=0;
	gettimeofday(&t, NULL);
	*time_ms=t.tv_sec*1000+t.tv_usec/1000;
	return EAR_SUCCESS;
}
state_t energy_ac_read(void *c, edata_t energy_mj)
{
	unsigned long long *pvalues=energy_mj;
  memset(pvalues,0,sizeof(unsigned long long)*RAPL_POWER_EVS*num_pack);

	return EAR_SUCCESS;
}

unsigned long long diff_RAPL_energy(unsigned long long init,unsigned long long end)
{
  unsigned long long ret=0;

  if (end>init){
    ret=end-init;
  }else{
    ret = ullong_diff_overflow(init, end);
  }
  return ret;
}

state_t energy_units(uint *units)
{
	*units=1000000000;
	return EAR_SUCCESS;
}
state_t energy_accumulated(unsigned long *e,edata_t init,edata_t end)
{
	
	int i;
	unsigned long total=0;
	unsigned long long diff;
	unsigned long long *pvalues_init,*pvalues_end;
	pvalues_init=(unsigned long long *)init;
	pvalues_end=(unsigned long long *)end;
	for (i=0;i<RAPL_POWER_EVS*num_pack;i++){
			diff=diff_RAPL_energy(pvalues_init[i],pvalues_end[i]);
			total+=diff;	
	}
	*e=total;
	return EAR_SUCCESS;
}


