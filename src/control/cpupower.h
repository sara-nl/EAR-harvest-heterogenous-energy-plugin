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

#ifndef _CPUPOWER_H

#define GOVERNOR_MAX_NAME_LEN 128

typedef struct governor{
	char name[GOVERNOR_MAX_NAME_LEN];
	unsigned long max_f,min_f;
}governor_t;

/* CPUPOWER API */

unsigned long CPUfreq_get_cpufreq_driver(int cpu,char *src);
unsigned long CPUfreq_get_cpufreq_governor(int cpu,char *src);
unsigned long CPUfreq_get_cpufreq_maxf(int cpu);
unsigned long CPUfreq_get_cpufreq_minf(int cpu);
void CPUfreq_set_cpufreq_governor(int fd,char *name);
unsigned long CPUfreq_set_cpufreq_minf(int cpu,unsigned long minf);
unsigned long CPUfreq_set_cpufreq_maxf(int cpu,unsigned long maxf);
unsigned long CPUfreq_set_frequency(unsigned int cpu, unsigned long f);
void CPUfreq_put_available_frequencies(unsigned long *f);
unsigned long *CPUfreq_get_available_frequencies(int cpu,unsigned long *num_freqs);
char **CPUfreq_get_available_governors(int cpu,unsigned long *num_governors);
void CPUfreq_put_available_governors(char **glist);
void CPUfreq_get_policy(governor_t *g);
void CPUfreq_set_policy(governor_t *g);
unsigned long CPUfreq_get_num_pstates(int cpu);
unsigned long CPUfreq_get(int cpu_num);


#endif
